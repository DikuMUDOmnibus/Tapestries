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



/*
 * Local functions.
 */
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt, long location ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA	*ch;
    CHAR_DATA	*ch_next;
    CHAR_DATA	*victim;
    CHAR_DATA	*rch;
    CHAR_DATA	*rch_next;

    for( ch = char_list ; ch ; ch = ch->next )
    {
	ch_next	= ch->next;

	if( !( victim = ch->fighting )
	 || !ch->in_room )
	    continue;

	if( IS_AWAKE( ch )
	 && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if( !( victim = ch->fighting ) )
	    continue;

        mprog_hitprcnt_trigger( ch, victim );
        mprog_fight_trigger( ch, victim );

	/*
	 * Fun for the whole family!
	 */
	for( rch = ch->in_room->people ; rch ; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

	    if( IS_AWAKE( rch ) && !rch->fighting )
	    {
		/*
		 * PC's auto-assist others in their group.
		 */
		if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) )
		{
		    if( ( !IS_NPC( rch ) || IS_AFFECTED( rch, AFF_CHARM ) )
		     && is_same_group( ch, rch ) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if( IS_NPC( rch ) && !IS_AFFECTED( rch, AFF_CHARM ) )
		{
		    if( rch->pIndexData == ch->pIndexData
		     || number_bits( 3 ) == 0 )
		    {
			CHAR_DATA	*vch;
			CHAR_DATA	*target;
			int		number;

			target = NULL;
			number = 0;
			for( vch = ch->in_room->people ; vch ; vch = vch->next )
			{
			    if( can_see( rch, vch )
			     && is_same_group( vch, victim )
			     && number_range( 0, number ) == 0 )
			    {
				target = vch;
				number++;
			    }
			}

			if( target ) /* TODO */
			{
			   if( !( IS_GOOD ( rch ) && IS_GOOD ( target ) )
			    || ( IS_EVIL ( rch ) || IS_EVIL ( target ) ) )
			     multi_hit( rch, target, TYPE_UNDEFINED );
			}
		    }
		}
	    }
	}
    }

    return;
}



/*
 * Do one group of attacks.
 * TODO a lot of work here...-Ant
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int		chance;

    one_hit( ch, victim, dt );
    if( ch->fighting != victim || dt == gsn_backstab )
	return;

    chance = IS_NPC(ch) ? 75 : ch->pcdata->learned[gsn_second_attack]/2;
    if( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? 75 : ch->pcdata->learned[gsn_third_attack]/4;
    if( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? 50 : 0;
    if( number_percent( ) < chance )
	one_hit( ch, victim, dt );
    
    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA	*wield;
    int		defence;
    int		attack;
    int		chance;
    int		dam;
    int		diceroll;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );
    if( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if( wield && IS_SET( wield->item_type, ITEM_WEAPON ) )
	    dt += wield->weapon->message;
    }

    /*
     * Calculate the various modifiers to-hit chance.
     */
    if( IS_NPC( victim ) )
    {
	defence	= 50;
    }
    else
    {
	defence	= victim->class->defence;
    }
    /*
     * modify defence chance.
     */
    defence += 2 * get_curr_dex( victim );
    if( !IS_AWAKE( victim ) )
	defence = 0;

    if( IS_NPC( ch ) )
    {
	attack	= 75;
    }
    else
    {
	attack	= ch->class->attack;
    }
    /*
     * modify attack chance.
     */
    attack += GET_HITROLL( ch );
    attack += get_curr_int( ch );
    attack += get_curr_dex( ch );
    if( !can_see( ch, victim ) )
	attack -= 20;


    chance = attack - defence;

    /*
     * Always a small chance.
     */
    if( chance < 5 )
	chance = 5;

    /*
     * The moment of excitement!
     */
    diceroll = number_percent( );

    /*
     * Missed attack.
     */
    if( diceroll >= 95
     || ( diceroll > chance ) )
    {
	if( diceroll >= 99 && number_bits( 3 ) == 0 )
	{
	    /* fumbled attack */
	}
	damage( ch, victim, 0, dt );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if( IS_NPC( ch ) )
    {
	dam = ( dice( ch->damage[ 0 ], ch->damage[ 1 ] ) + ch->damage[ 2 ] );
	if( wield )
	    dam += ( dice( wield->weapon->damage[0], wield->weapon->damage[1] )
			+ wield->weapon->damage[2] );
    }
    else
    {
	if( wield )
	    dam = ( dice( wield->weapon->damage[0], wield->weapon->damage[1] )
			+ wield->weapon->damage[2] );
	else
	    dam = number_range( 1, 4 );
    }

    /*
     * Bonuses.
     */
    dam += GET_DAMROLL( ch );

    if( !IS_NPC( ch )
     && ch->pcdata->learned[ gsn_enhanced_damage ] > 0 )
	dam += dam * ch->pcdata->learned[ gsn_enhanced_damage ] / 150;
    if( !IS_AWAKE( victim ) )
	dam *= 2;
    if( dt == gsn_backstab )
	dam *= 2 + ( ch->pcdata->learned[ gsn_backstab ] / 50 );

    if( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, dt );
    tail_chain( );
    return;
}



/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    PART_DATA	*location = NULL;
    int		part;
    int		i;

    if( victim->position == POS_DEAD )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if( dam > 30000 )
    {
	bug( "Damage: %d: more than 30000 points!", dam );
	dam = 30000;
    }

    if( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if( victim->position > POS_STUNNED )
	{
	    if( !victim->fighting )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	}

	if( victim->position > POS_STUNNED )
	{
	    if( !ch->fighting )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if( IS_NPC( ch )
	     && IS_NPC( victim )
	     && IS_AFFECTED( victim, AFF_CHARM )
	     && victim->master
	     && victim->master->in_room == ch->in_room
	     && number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Inviso attacks ... not.
	 */
	if( IS_AFFECTED( ch, AFF_INVISIBLE ) )
	{
	    affect_strip( ch, gsn_invis );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	    act_new( "$n fades into existence.",
		ch, NULL, NULL, TO_ROOM, POS_RESTING );
	}

	/*
	 * Damage modifiers.
	 */
	if( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	    dam /= 2;

	if( IS_AFFECTED( victim, AFF_PROTECT ) && IS_EVIL( ch ) )
	    dam -= dam / 4;

	/*
	 * Check for disarm, trip, parry, and dodge. TODO
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) && number_percent( ) < 25 )
		disarm( ch, victim );
	    if ( IS_NPC(ch) && number_percent( ) < 50 )
		trip( ch, victim );
	    if ( check_parry( ch, victim ) )
		return;
	    if ( check_dodge( ch, victim ) )
		return;
	}

	if( !IS_NPC( victim ) )
	{
	    part	= number_range( 0, 6 );
	    i		= 0;
	    for( location = victim->parts ; location ; location = location->next )
	    {
		if( i++ == part )
		    break;
	    }
	    dam -= location->armour;
	}

	if( dam < 0 )
	    dam = 0;

	if( !IS_NPC( victim ) )
	    dam_message( ch, victim, dam, dt, location->type );
	else
	    dam_message( ch, victim, dam, dt, -99 );
    }

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	if( dam > victim->max_hit / 4 )
	{
	    if( location )
	    {
		location->scars++;
		switch( location->type )
		{
		    case PART_HEAD:
			send_to_char( "You take a nasty blow to the head,\n\r", victim );
			break;
		    case PART_CHEST:
			send_to_char( "You take a nasty wound to the chest,\n\r", victim );
			break;
		    case PART_ABDOMIN:
			send_to_char( "You take a nasty wound in the guts,\n\r", victim );
			break;
		    case PART_LEFT_ARM:
			send_to_char( "You take a nasty wound to the left arm,\n\r", victim );
			break;
		    case PART_RIGHT_ARM:
			send_to_char( "You take a nasty wound to the right arm,\n\r", victim );
			break;
		    case PART_LEFT_LEG:
			send_to_char( "You take a nasty wound to the left leg,\n\r", victim );
			break;
		    case PART_RIGHT_LEG:
			send_to_char( "You take a nasty wound to the right leg,\n\r", victim );
			break;
		    case PART_LFRONT_LEG:
			send_to_char( "You take a nasty wound to the left front leg,\n\r", victim );
			break;
		    case PART_RFRONT_LEG:
			send_to_char( "You take a nasty wound to the right front leg,\n\r", victim );
			break;
		    case PART_LREAR_LEG:
			send_to_char( "You take a nasty wound to the left back leg,\n\r", victim );
			break;
		    case PART_RREAR_LEG:
			send_to_char( "You take a nasty wound to the right back leg,\n\r", victim );
			break;
		    case PART_LEFT_WING:
			send_to_char( "You take a nasty wound to the left wing,\n\r", victim );
			break;
		    case PART_RIGHT_WING:
			send_to_char( "You take a nasty wound to the right wing,\n\r", victim );
			break;
		    case PART_TAIL:
			send_to_char( "You take a nasty wound to the tail,\n\r", victim );
			break;
		    default:
			send_to_char( "You take a nasty wound,\n\r", victim );
			break;
		}
	    }
	    send_to_char( "That really did HURT!\n\r", victim );
	}
	if( victim->hit < victim->max_hit / 4 )
	    send_to_char( "You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if( !IS_AWAKE( victim ) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if( !IS_NPC( victim ) )
	{
	    int		stat;
	    int		loss;

	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		( IS_NPC( ch ) ? ch->short_descr : ch->name ),
		victim->in_room->vnum );
	    log_string( log_buf );

	    /*
	     * Dying penalty:
	     * 6-10 points lost from a random stat.
	     */
	    stat = number_range( 0, 4 );
	    loss = number_range( 6, 10 );
	    if( stat == 0 )
	    {
		send_to_char( "You feel weaker...\n\r", ch );
		victim->pcdata->perm_str -= loss;
		if( victim->pcdata->perm_str < 1 )
		    victim->pcdata->perm_str = 1;
	    }
	    else if( stat == 1 )
	    {
		send_to_char( "You feel a loss of awareness...\n\r", ch );
		victim->pcdata->perm_int -= loss;
		if( victim->pcdata->perm_int < 1 )
		    victim->pcdata->perm_int = 1;
	    }
	    else if( stat == 2 )
	    {
		send_to_char( "You feel less wise...\n\r", ch );
		victim->pcdata->perm_wis -= loss;
		if( victim->pcdata->perm_wis < 1 )
		    victim->pcdata->perm_wis = 1;
	    }
	    else if( stat == 3 )
	    {
		send_to_char( "You feel less deft...\n\r", ch );
		victim->pcdata->perm_dex -= loss;
		if( victim->pcdata->perm_dex < 1 )
		    victim->pcdata->perm_dex = 1;
	    }
	    else if( stat == 4 )
	    {
		send_to_char( "You feel sick...\n\r", ch );
		victim->pcdata->perm_con -= loss;
		if( victim->pcdata->perm_con < 1 )
		    victim->pcdata->perm_con = 1;
	    }
	}

	raw_kill( victim );

	if( !IS_NPC( ch ) && IS_NPC( victim ) )
	{
	    if( IS_SET( ch->act, PLR_AUTOLOOT ) )
		do_get( ch, "all corpse" );
	    else
		do_look( ch, "in corpse" );

	    if( IS_SET(ch->act, PLR_AUTOSAC) )
		do_sacrifice( ch, "corpse" );
	}

	return;
    }

    if( victim == ch )
	return;

    /*
     * Take care of link dead people.
     */
    if( !IS_NPC( victim )
     && !victim->desc )
    {
	if( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }

    /*
     * Wimp out?
     */
    if( IS_NPC( victim ) && dam > 0 )
    {
	if( ( IS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
	 && victim->hit < victim->max_hit / 2 )
	 || ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master
	   && victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if( !IS_NPC( victim )
     && victim->hit > 0
     && victim->hit <= victim->wimpy
     && victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;

    if( get_age( ch ) < 21 )
    {
	send_to_char( "You aren't old enough.\n\r", ch );
	return TRUE;
    }

    if( IS_SET( victim->act, PLR_KILLER ) )
        return FALSE;

    return FALSE;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim)
    ||   IS_SET(victim->act, PLR_KILLER)
    ||   IS_SET(victim->act, PLR_THIEF) )
	return;

    /*
     * Charm-o-rama.
     */
    if ( IS_SET(ch->affected_by, AFF_CHARM) )
    {
	if ( ch->master == NULL )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Check_killer: %s bad AFF_CHARM",
		IS_NPC(ch) ? ch->short_descr : ch->name );
	    bug( buf, 0 );
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}

	send_to_char( "*** You are now a KILLER!! ***\n\r", ch->master );
	SET_BIT(ch->master->act, PLR_KILLER);
	stop_follower( ch );
	return;
    }

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch)
    ||   ch == victim
    ||   IS_SET(ch->act, PLR_KILLER) )
	return;

    send_to_char( "*** You are now a KILLER!! ***\n\r", ch );
    SET_BIT(ch->act, PLR_KILLER);
    save_char_obj( ch );
    return;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
    {
	chance	= 50; /* TODO */
    }
    else
    {
	if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
	    return FALSE;
	chance	= victim->pcdata->learned[gsn_parry] / 2;
    }

    if ( number_percent( ) >= chance )
	return FALSE;

    act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
        chance  = 50;
    else
        chance  = victim->pcdata->learned[gsn_dodge] / 2;

    if ( number_percent( ) >= chance )
        return FALSE;

    act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
    act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) || victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if( IS_NPC( ch ) )
    {
	name		= ch->short_descr;
	corpse		= create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ) );
	corpse->condition = 100;
	corpse->timer	= number_range( 2, 4 );
	if( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold ), corpse );
	    ch->gold = 0;
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ) );
	corpse->condition = 100;
	corpse->timer	= number_range( 15, 20 );
    }

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for( obj = ch->carrying ; obj ; obj = obj_next )
    {
	obj_next = obj->next_content;
	obj_from_char( obj );
	if( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else
	    obj_to_obj( obj, corpse );
    }

    obj_to_room( corpse, ch->in_room );
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    switch ( number_bits( 4 ) )
    {
    default: msg  = "You hear $n's death cry.";				break;
    case  0: msg  = "$n hits the ground ... DEAD.";			break;
    case  1: msg  = "$n splatters blood on your armour.";		break;
    case  2: msg  = "You smell $n's sphincter releasing in death.";
	     vnum = OBJ_VNUM_FINAL_TURD;				break;
    case  3: msg  = "$n's severed head plops on the ground.";
	     vnum = OBJ_VNUM_SEVERED_HEAD;				break;
    case  4: msg  = "$n's beating heart is torn from $s chest.";
	     vnum = OBJ_VNUM_TORN_HEART;				break;
    case  5: msg  = "$n's arm is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_ARM;				break;
    case  6: msg  = "$n's leg is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_LEG;				break;
    }

    act_new( msg, ch, NULL, NULL, TO_ROOM, POS_RESTING );

    if( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ) );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	obj_to_room( obj, ch->in_room );
    }

    if( IS_NPC( ch ) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for( door = 0 ; door <= MAX_DIR ; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim )
{
    stop_fighting( victim, TRUE );
    mprog_death_trigger( victim );
    make_corpse( victim );

    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, 50, MAX_LEVEL-1)].killed++; /* TODO */
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by	= 0;
    victim->armour	= 100;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    victim->move	= UMAX( 1, victim->move );
    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC(ch) || !IS_NPC(victim) || victim == ch )
	return;
    
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) )
	    continue;

	xp = xp_compute( gch, victim ) / members;
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
		act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * New version for Tapestries by Lope
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int xp;
    int base_exp;
    int align;

    if( IS_NPC( victim ) )
    {
	base_exp	= victim->exp;
    }
    else
    {
	base_exp	= 0; /* TODO xp for pc killing */
    }

    align = victim->alignment - gch->alignment;
    /* TODO alignment changing for kills */

    if( gch->alignment > 1000 )
	gch->alignment = 1000;
    else if( gch->alignment < -1000 )
	gch->alignment = -1000;

    /* TODO adjustment of xp for differences in align */

    /* Add a bit of fuzz to the xp gained */
    xp = number_range( base_exp * 3/4, base_exp * 5/4 );

    /* TODO adjustment for grouping? */

    return xp;
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, long location )
{
    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grep", "bite",
	"pierce", "suction"
    };

    char	buf1[ 256 ];
    char	buf2[ 256 ];
    char	buf3[ 256 ];
    const char	*where;
    const char	*vs;
    const char	*vp;
    const char	*attack;
    char	punct;

    if( location != -99 )
	where = flag_string( part_flags, location );
    else
	where = "torso"; /* TODO */

	 if( dam ==   0 ) { vs = "miss";	vp = "misses";		}
    else if( dam <=   4 ) { vs = "scratch";	vp = "scratches";	}
    else if( dam <=   8 ) { vs = "graze";	vp = "grazes";		}
    else if( dam <=  12 ) { vs = "hit";		vp = "hits";		}
    else if( dam <=  16 ) { vs = "injure";	vp = "injures";		}
    else if( dam <=  20 ) { vs = "wound";	vp = "wounds";		}
    else if( dam <=  24 ) { vs = "maul";	vp = "mauls";		}
    else if( dam <=  28 ) { vs = "decimate";	vp = "decimates";	}
    else if( dam <=  32 ) { vs = "devastate";	vp = "devastates";	}
    else if( dam <=  36 ) { vs = "maim";	vp = "maims";		}
    else if( dam <=  40 ) { vs = "MUTILATE";	vp = "MUTILATES";	}
    else if( dam <=  44 ) { vs = "DISEMBOWEL";	vp = "DISEMBOWELS";	}
    else if( dam <=  48 ) { vs = "EVISCERATE";	vp = "EVISCERATES";	}
    else if( dam <=  52 ) { vs = "MASSACRE";	vp = "MASSACRES";	}
    else if( dam <= 100 ) { vs = "*** DEMOLISH ***";
			    vp = "*** DEMOLISHES ***";			}
    else                  { vs = "*** ANNIHILATE ***";
			    vp = "*** ANNIHILATES ***";			}

    punct   = ( dam <= 24 ) ? '.' : '!';


    if( dt == TYPE_HIT )
    {
	sprintf( buf1, "{3$n %s $N's %s%c{x",  vp, where, punct );
	sprintf( buf2, "{2You %s $N's %s%c{x", vs, where, punct );
	sprintf( buf3, "{4$n %s your %s%c{x", vp, where, punct );
    }
    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	    attack	= skill_table[dt].noun_damage;
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	    attack	= attack_table[dt - TYPE_HIT];
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0];
	}

	sprintf( buf1, "{3$n's %s %s $N's %s%c{x",  attack, vp, where, punct );
	sprintf( buf2, "{2Your %s %s $N's %s%c{x",  attack, vp, where, punct );
	sprintf( buf3, "{4$n's %s %s your %s%c{x", attack, vp, where, punct );
    }

    act_new( buf1, ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    act_new( buf2, ch, NULL, victim, TO_CHAR, POS_RESTING );
    act_new( buf3, ch, NULL, victim, TO_VICT, POS_RESTING );

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

   if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
     return;

 
    act( "$n DISARMS you!", ch, NULL, victim, TO_VICT    );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n DISARMS $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_NPC(victim) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->wait == 0 )
    {
	act( "$n trips you and you go down!", ch, NULL, victim, TO_VICT    );
	act( "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR    );
	act( "$n trips $N and $N goes down!", ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	if ( !IS_SET(victim->act, PLR_KILLER)
	&&   !IS_SET(victim->act, PLR_THIEF) )
	{
	    send_to_char( "You must MURDER a player.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	{
	    send_to_char( "You must MURDER a charmed creature.\n\r", ch );
	    return;
	}
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
    do_shout( victim, buf );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];

/* TODO */

    one_argument( argument, arg );
    
    if( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if( is_safe( ch, victim ) )
      return;

    if( !( obj = get_eq_char( ch, WEAR_WIELD ) ) )
    {
	send_to_char( "You need to wield a weapon.\n\r", ch );
	return;
    }

    if( victim->fighting )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if( victim->hit < victim->max_hit )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if( !IS_AWAKE( victim )
     || IS_NPC( ch )
     || number_percent( ) < ch->pcdata->learned[gsn_backstab] )
	multi_hit( ch, victim, gsn_backstab );
    else
	damage( ch, victim, 0, gsn_backstab );

    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	    ||   pexit->to_room == NULL
	    ||   IS_SET(pexit->exit_info, EX_CLOSED)
	    || ( IS_NPC(ch)
		&& ( IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		    || ( IS_SET(ch->act, ACT_STAY_AREA)
			&& pexit->to_room->area != ch->in_room->area ) ) ) )
	    continue;

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if ( !IS_NPC(ch) )
	{
	    send_to_char( "You flee from combat!  You lose 25 exps.\n\r", ch );
	    gain_exp( ch, -25 );
	}

	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "You failed!  You lose 10 exps.\n\r", ch );
    gain_exp( ch, -10 );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

/* TODO */

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_rescue] )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, fch );
    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

/* TODO */

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
	damage( ch, victim, number_range( 1, ch->pcdata->learned[gsn_kick] ), gsn_kick );
    else
	damage( ch, victim, 0, gsn_kick );

    return;
}




void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

/* TODO */

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( );
    if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 )
	disarm( ch, victim );
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( victim );
    return;
}
