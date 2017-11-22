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
#define CD CHAR_DATA
void	get_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			    OBJ_DATA *container ) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			    bool fReplace, long location ) );
CD *	find_keeper	args( ( CHAR_DATA *ch ) );
int	get_cost	args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
#undef	CD




void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( "$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	act( "$d: you can't carry that much weight.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if( container )
    {
	act( "You get $p from $P.", ch, obj, container, TO_CHAR );
	act( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
	obj_from_obj( obj );
    }
    else
    {
	act( "You get $p.", ch, obj, container, TO_CHAR );
	act( "$n gets $p.", ch, obj, container, TO_ROOM );
	obj_from_room( obj );
    }

    if( IS_SET( obj->item_type, ITEM_MONEY ) )
    {
	ch->gold += obj->money->gold;
/*	ch->silver += obj->money->silver;
	ch->copper += obj->money->copper;	Not yet...-Ant */
	REMOVE_BIT( obj->item_type, ITEM_MONEY );
	if( obj->item_type == 0 )
	    extract_obj( obj );
    }
    else
    {
	obj_to_char( obj, ch );
    }

    return;
}



void do_get( CHAR_DATA *ch, char *argument )
{
    char	arg1[MAX_INPUT_LENGTH];
    char	arg2[MAX_INPUT_LENGTH];
    OBJ_DATA	*obj;
    OBJ_DATA	*obj_next;
    OBJ_DATA	*container;
    bool	found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /* Get type. */
    if( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

    if( arg2[0] == '\0' )
    {
	if( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	    if( !obj )
	    {
		act( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
		return;
	    }

	    get_obj( ch, obj, NULL );
	}
	else
	{
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if( ( arg1[3] == '\0'
		 || is_name( &arg1[4], obj->name ) )
		 && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, NULL );
		}
	    }

	    if( !found ) 
	    {
		if( arg1[3] == '\0' )
		    send_to_char( "I see nothing here.\n\r", ch );
		else
		    act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
	    }
	}
    }
    else
    {
	/* 'get ... container' */
	if( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if( !( container = get_obj_here( ch, arg2 ) ) )
	{
	    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	if( IS_SET( container->item_type, ITEM_CONTAINER )
	 || IS_SET( container->item_type, ITEM_CORPSE_NPC )
	 || IS_SET( container->item_type, ITEM_CORPSE_PC ) )
	    ;
	else
	{
	    send_to_char( "That's not a container.\n\r", ch );
	    return;
	}	    

	if( IS_SET( container->item_type, ITEM_CONTAINER )
	 && IS_SET( container->cont->closed, CONT_CLOSED ) )
	{
	    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->contains );
	    if( !obj  )
	    {
		act( "I see nothing like that in the $T.",
		    ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    get_obj( ch, obj, container );
	}
	else
	{
	    /* 'get all container' or 'get all.obj container' */
	    found = FALSE;
	    for( obj = container->contains ; obj ; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		 && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, container );
		}
	    }

	    if( !found )
	    {
		if( arg1[3] == '\0' )
		    act( "I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		    act( "I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	}
    }

    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

    if( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
	act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	return;
    }

    if( container->item_type != ITEM_CONTAINER )
    {
	send_to_char( "That's not a container.\n\r", ch );
	return;
    }

    if( IS_SET( container->cont->closed, CONT_CLOSED ) )
    {
	act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	return;
    }

    if( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	if( ( get_obj_weight( obj ) +
	       get_obj_weight( container ) > container->cont->capacity )
	 || ( get_obj_weight( obj ) > container->cont->single ) )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
	act( "You put $p in $P.", ch, obj, container, TO_CHAR );
    }
    else
    {
	/* 'put all container' or 'put all.obj container' */
	for( obj = ch->carrying ; obj ; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
	     && can_see_obj( ch, obj )
	     && obj->wear_loc == WEAR_NONE
	     && obj != container
	     && can_drop_obj( ch, obj )
	     && get_obj_weight( obj ) + get_obj_weight( container )
		 <= container->cont->capacity
	     && get_obj_weight( obj ) <= container->cont->single )
	    {
		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
		act( "You put $p in $P.", ch, obj, container, TO_CHAR );
	    }
	}
    }

    return;
}



void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }

    if( is_number( arg ) )
    {
	/* 'drop NNNN coins' */
	int amount;

	amount   = atoi(arg);
	argument = one_argument( argument, arg );
	if( amount <= 0
	 || ( str_cmp( arg, "coins" ) && str_cmp( arg, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	if( ch->gold < amount )
	{
	    send_to_char( "You haven't got that many coins.\n\r", ch );
	    return;
	}

	ch->gold -= amount;

	for( obj = ch->in_room->contents ; obj ; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    switch ( obj->pIndexData->vnum )
	    {
	    case OBJ_VNUM_MONEY_ONE:
		amount += 1;
		extract_obj( obj );
		break;

	    case OBJ_VNUM_MONEY_SOME:
		amount += obj->money->gold;
		extract_obj( obj );
		break;
	    }
	}

	obj_to_room( create_money( amount ), ch->in_room );
	act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "OK.\n\r", ch );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	/* 'drop obj' */
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
	act( "You drop $p.", ch, obj, NULL, TO_CHAR );
    }
    else
    {
	/* 'drop all' or 'drop all.obj' */
	found = FALSE;
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
		act( "You drop $p.", ch, obj, NULL, TO_CHAR );
	    }
	}

	if ( !found )
	{
	    if ( arg[3] == '\0' )
		act( "You are not carrying anything.",
		    ch, NULL, arg, TO_CHAR );
	    else
		act( "You are not carrying any $T.",
		    ch, NULL, &arg[4], TO_CHAR );
	}
    }

    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	/* 'give NNNN coins victim' */
	int amount;

	amount   = atoi(arg1);
	if ( amount <= 0
	|| ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give what to whom?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that much gold.\n\r", ch );
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
	act( "$n gives you some gold.", ch, NULL, victim, TO_VICT    );
	act( "$n gives $N some gold.",  ch, NULL, victim, TO_NOTVICT );
	act( "You give $N some gold.",  ch, NULL, victim, TO_CHAR    );
	send_to_char( "OK.\n\r", ch );
	mprog_bribe_trigger(victim,ch,amount);
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    MOBtrigger = FALSE;
    act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    act( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
    act( "You give $p to $N.", ch, obj, victim, TO_CHAR    );
    mprog_give_trigger(victim,ch,obj);
    return;
}




void do_fill( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*fountain;
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    bool	found;

    one_argument( argument, arg );

    if( !arg )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }

    if( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    found = FALSE;
    for( fountain = ch->in_room->contents ;
	 fountain ;
	 fountain = fountain->next_content )
    {
	if( IS_SET( fountain->item_type, ITEM_FOUNTAIN ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if( !found )
    {
	send_to_char( "There is no fountain here!\n\r", ch );
	return;
    }

    if( !IS_SET( obj->item_type, ITEM_DRINK_CON ) )
    {
	send_to_char( "You can't fill that.\n\r", ch );
	return;
    }

    if( obj->drink->volume != 0 && obj->drink->liquid != 0 )
    {
	send_to_char( "There is already another liquid in it.\n\r", ch );
	return;
    }

    if( obj->drink->volume >= obj->drink->limit )
    {
	send_to_char( "Your container is full.\n\r", ch );
	return;
    }

    act( "You fill $p.", ch, obj, NULL, TO_CHAR );
    obj->drink->liquid = 0;
    obj->drink->volume = obj->drink->limit;

    return;
}



void do_drink( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];
    int		amount;
    int		liquid;

    one_argument( argument, arg );

    if( !arg )
    {
	for( obj = ch->in_room->contents ; obj ; obj = obj->next_content )
	{
	    if( IS_SET( obj->item_type, ITEM_FOUNTAIN ) )
		break;
	}

	if( !obj )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if( !( obj = get_obj_here( ch, arg ) ) )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
    {
	send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
	return;
    }

    if( IS_SET( obj->item_type, ITEM_FOUNTAIN ) )
    {
	if( !IS_NPC( ch ) )
	    ch->pcdata->condition[COND_THIRST] = 48;
	act( "$n drinks from the fountain.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You are not thirsty.\n\r", ch );
    }
    else if( IS_SET( obj->item_type, ITEM_DRINK_CON ) )
    {
	if( obj->drink->volume <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if( ( liquid = obj->drink->liquid ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->drink->liquid = 0;
	}

	act( "$n drinks $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You drink $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR );

	amount = number_range( 3, 10 );
	amount = UMIN( amount, obj->drink->volume );
	
	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );
	gain_condition( ch, COND_FULL,
	    amount * liq_table[liquid].liq_affect[COND_FULL   ] );
	gain_condition( ch, COND_THIRST,
	    amount * liq_table[liquid].liq_affect[COND_THIRST ] );

	if( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
	    send_to_char( "You feel drunk.\n\r", ch );
	if( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   > 40 )
	    send_to_char( "You are full.\n\r", ch );
	if( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
	    send_to_char( "You do not feel thirsty.\n\r", ch );
	
	if( obj->drink->poison != 0 )
	{
	    /* The shit was poisoned ! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );
	    af.type      = gsn_poison;
	    af.duration  = 3 * amount;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	
	obj->drink->volume -= amount;

	if( obj->drink->volume < 0 )
	    obj->drink->volume = 0;
    }
    else
    {
	send_to_char( "You can't drink from that.\n\r", ch );
    }

    return;
}



void do_eat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if( !arg )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if( !IS_IMMORTAL( ch ) )
    {
	if( !IS_SET( obj->item_type, ITEM_FOOD )
	 && !IS_SET( obj->item_type, ITEM_PILL ) )
	{
	    send_to_char( "That's not edible.\n\r", ch );
	    return;
	}

	if( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 40 )
	{   
	    send_to_char( "You are too full to eat more.\n\r", ch );
	    return;
	}
    }

    act( "$n eats $p.",  ch, obj, NULL, TO_ROOM );
    act( "You eat $p.", ch, obj, NULL, TO_CHAR );

    if( IS_SET( obj->item_type, ITEM_FOOD ) )
    {
	if( !IS_NPC( ch ) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, obj->food->bite );
	    if( condition == 0 && ch->pcdata->condition[COND_FULL] > 0 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	    else if( ch->pcdata->condition[COND_FULL] > 40 )
		send_to_char( "You are full.\n\r", ch );
	}

	if( obj->food->poison != 0 )
	{
	    /* The shit was poisoned! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, 0, 0, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );

	    af.type      = gsn_poison;
	    af.duration  = 2 * obj->food->bite;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
    }

    if( IS_SET( obj->item_type, ITEM_PILL ) )
    {
	obj_cast_spell( obj->pill->spell[0], obj->pill->level, ch, ch, NULL );
	obj_cast_spell( obj->pill->spell[1], obj->pill->level, ch, ch, NULL );
	obj_cast_spell( obj->pill->spell[2], obj->pill->level, ch, ch, NULL );
	obj_cast_spell( obj->pill->spell[3], obj->pill->level, ch, ch, NULL );
    }

    extract_obj( obj );
    return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
    {
	act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 * Improved and adapted for multiple wear locations.
 * -Ant
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, long location )
{
    if( get_curr_str( ch ) < obj->requires[ 0 ] )
    {
	send_to_char_bw( "You are not strong enough to use this object.\n\r", ch );
	act_new( "$n tries to use $p, but is too weak.",
	    ch, obj, NULL, TO_ROOM, POS_RESTING );
	return;
    }
    else if( get_curr_int( ch ) < obj->requires[ 1 ] )
    {
	send_to_char_bw( "You are not intellegent enough to use this object.\n\r", ch );
	act_new( "$n tries to use $p, but is too stupid.",
	    ch, obj, NULL, TO_ROOM, POS_RESTING );
	return;
    }
    else if( get_curr_wis( ch ) < obj->requires[ 2 ] )
    {
	send_to_char_bw( "You are not wise enough to use this object.\n\r", ch );
	act_new( "$n tries to use $p, but is too rash.",
	    ch, obj, NULL, TO_ROOM, POS_RESTING );
	return;
    }
    else if( get_curr_dex( ch ) < obj->requires[ 3 ] )
    {
	send_to_char_bw( "This object is too complicated for you.\n\r", ch );
	act_new( "$n tries to use $p, but is too clumsy.",
	    ch, obj, NULL, TO_ROOM, POS_RESTING );
	return;
    }
    else if( get_curr_con( ch ) < obj->requires[ 4 ] )
    {
	send_to_char_bw( "You are not robust enough to use this object.\n\r", ch );
	act_new( "$n tries to use $p, but cannot.",
	    ch, obj, NULL, TO_ROOM, POS_RESTING );
	return;
    }

    switch( location )
    {
	case WEAR_LIGHT:
	    if( IS_SET( obj->item_type, ITEM_LIGHT ) )
	    {
		if( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
		    return;
		act_new( "$n lights $p and holds it.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You light $p and hold it.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_LIGHT );
		return;
	    }
	    break;

	case WEAR_FINGER_L:
	    if( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
	    {
		if( get_eq_char( ch, WEAR_FINGER_L )
		 && !remove_obj( ch, WEAR_FINGER_L, fReplace ) )
		    return;

		if( !get_eq_char( ch, WEAR_FINGER_L ) )
		{
		    act_new( "$n wears $p on $s left finger.",
			ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_new( "You wear $p on your left finger.",
			ch, obj, NULL, TO_CHAR, POS_RESTING );
		    equip_char( ch, obj, WEAR_FINGER_L );
		    return;
		}

		bug( "Wear_obj: left finger not free.", 0 );
		send_to_char( "You already wear a ring there.\n\r", ch );
		return;
	    }
	    break;

	case WEAR_FINGER_R:
	    if( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
	    {
		if( get_eq_char( ch, WEAR_FINGER_R )
		 && !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
		    return;

		if( !get_eq_char( ch, WEAR_FINGER_R ) )
		{
		    act_new( "$n wears $p on $s right finger.",
			ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_new( "You wear $p on your right finger.",
			ch, obj, NULL, TO_CHAR, POS_RESTING );
		    equip_char( ch, obj, WEAR_FINGER_R );
		    return;
		}

		bug( "Wear_obj: right finger not free.", 0 );
		send_to_char( "You already wear a ring there.\n\r", ch );
		return;
	    }
	    break;

	case WEAR_NECK_1:
	    if( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
	    {
		if( get_eq_char( ch, WEAR_NECK_1 )
		 && !remove_obj( ch, WEAR_NECK_1, fReplace ) )
		    return;

		if( !get_eq_char( ch, WEAR_NECK_1 ) )
		{
		    act_new( "$n wears $p around $s neck.",
			ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_new( "You wear $p around your neck.",
			ch, obj, NULL, TO_CHAR, POS_RESTING );
		    equip_char( ch, obj, WEAR_NECK_1 );
		    return;
		}

		bug( "Wear_obj: no free neck.", 0 );
		send_to_char( "You already wear a neck item there.\n\r", ch );
		return;
	    }
	    break;

	case WEAR_NECK_2:
	    if( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
	    {
		if( get_eq_char( ch, WEAR_NECK_2 )
		 && !remove_obj( ch, WEAR_NECK_2, fReplace ) )
		    return;

		if( !get_eq_char( ch, WEAR_NECK_2 ) )
		{
		    act_new( "$n wears $p around $s neck.",
			ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_new( "You wear $p around your neck.",
			ch, obj, NULL, TO_CHAR, POS_RESTING );
		    equip_char( ch, obj, WEAR_NECK_2 );
		    return;
		}

		bug( "Wear_obj: no free neck.", 0 );
		send_to_char( "You already wear a neck item there.\n\r", ch );
		return;
	    }
	    break;

	case WEAR_BODY:
	    if( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
	    {
		if( !remove_obj( ch, WEAR_BODY, fReplace ) )
		    return;
		act_new( "$n wears $p on $s body.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p on your body.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_BODY );
		return;
	    }
	    break;

	case WEAR_HEAD:
	    if( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
	    {
		if( !remove_obj( ch, WEAR_HEAD, fReplace ) )
		    return;
		act_new( "$n wears $p on $s head.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p on your head.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_HEAD );
		return;
	    }
	    break;

	case WEAR_LEGS:
	    if( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
	    {
		if( !remove_obj( ch, WEAR_LEGS, fReplace ) )
		    return;
		act_new( "$n wears $p on $s legs.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p on your legs.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_LEGS );
		return;
	    }
	    break;

	case WEAR_FEET:
	    if( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
	    {
		if( !remove_obj( ch, WEAR_FEET, fReplace ) )
		    return;
		act_new( "$n wears $p on $s feet.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p on your feet.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_FEET );
		return;
	    }
	    break;

	case WEAR_HANDS:
	    if( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
	    {
		if( !remove_obj( ch, WEAR_HANDS, fReplace ) )
		    return;
		act_new( "$n wears $p on $s hands.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p on your hands.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_HANDS );
		return;
	    }
	    break;

	case WEAR_ARMS:
	    if( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
	    {
		if( !remove_obj( ch, WEAR_ARMS, fReplace ) )
		    return;
		act_new( "$n wears $p on $s arms.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p on your arms.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_ARMS );
		return;
	    }
	    break;

	case WEAR_SHIELD:
	    if( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
	    {
		if( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
		    return;
		act_new( "$n wears $p as a shield.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p as a shield.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_SHIELD );
		return;
	    }
	    break;

	case WEAR_ABOUT:
	    if( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
	    {
		if( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
		    return;
		act_new( "$n wears $p about $s body.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p about your body.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_ABOUT );
		return;
	    }
	    break;

	case WEAR_WAIST:
	    if( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
	    {
		if( !remove_obj( ch, WEAR_WAIST, fReplace ) )
		    return;
		act_new( "$n wears $p about $s waist.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wear $p about your waist.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_WAIST );
		return;
	    }
	    break;

	case WEAR_WRIST_L:
	    if( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
	    {
		if( get_eq_char( ch, WEAR_WRIST_L )
		 && !remove_obj( ch, WEAR_WRIST_L, fReplace ) )
		    return;

		if( !get_eq_char( ch, WEAR_WRIST_L ) )
		{
		    act_new( "$n wears $p around $s left wrist.",
			ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_new( "You wear $p around your left wrist.",
			ch, obj, NULL, TO_CHAR, POS_RESTING );
		    equip_char( ch, obj, WEAR_WRIST_L );
		    return;
		}
	
		bug( "Wear_obj: no free wrist.", 0 );
		send_to_char( "You already wear a wrist item there.\n\r", ch );
		return;
	    }
	    break;

	case WEAR_WRIST_R:
	    if( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
	    {
		if( get_eq_char( ch, WEAR_WRIST_R )
		 && !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
		    return;

		if( !get_eq_char( ch, WEAR_WRIST_R ) )
		{
		    act_new( "$n wears $p around $s right wrist.",
			ch, obj, NULL, TO_ROOM, POS_RESTING );
		    act_new( "You wear $p around your right wrist.",
			ch, obj, NULL, TO_CHAR, POS_RESTING );
		    equip_char( ch, obj, WEAR_WRIST_R );
		    return;
		}

		bug( "Wear_obj: no free wrist.", 0 );
		send_to_char( "You already wear a wrist item there.\n\r", ch );
		return;
	    }
	    break;

	case WEAR_WIELD:
	    if( CAN_WEAR( obj, ITEM_WIELD ) )
	    {
		if( !remove_obj( ch, WEAR_WIELD, fReplace ) )
		    return;
	
		if( get_obj_weight( obj ) > ( get_curr_str( ch ) / 2 ) )
		{
		    send_to_char( "It is too heavy for you to wield.\n\r", ch );
		    return;
		}

		act_new( "$n wields $p.", ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You wield $p.", ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_WIELD );
		return;
	    }
	    break;

	case WEAR_HOLD:
	    if( CAN_WEAR( obj, ITEM_HOLD ) )
	    {
		if( !remove_obj( ch, WEAR_HOLD, fReplace ) )
		    return;
		act_new( "$n holds $p in $s hands.",
		    ch, obj, NULL, TO_ROOM, POS_RESTING );
		act_new( "You hold $p in your hands.",
		    ch, obj, NULL, TO_CHAR, POS_RESTING );
		equip_char( ch, obj, WEAR_HOLD );
		return;
	    }
	    break;

	case NO_FLAG:
	case WEAR_NONE:
	default:
	    break;
    }


    if( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;
	act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
	act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LIGHT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {
	if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
	&&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
	&&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	&&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
	{
	    act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
	{
	    act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_FINGER_R );
	    return;
	}

	bug( "Wear_obj: no free finger.", 0 );
	send_to_char( "You already wear two rings.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
	if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	&&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
	&&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	&&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
	{
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_1 );
	    return;
	}

	if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
	{
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_NECK_2 );
	    return;
	}

	bug( "Wear_obj: no free neck.", 0 );
	send_to_char( "You already wear two neck items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	    return;
	act( "$n wears $p on $s body.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your body.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_BODY );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	    return;
	act( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HEAD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	    return;
	act( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_LEGS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	    return;
	act( "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_FEET );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	    return;
	act( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HANDS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	    return;
	act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ARMS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	    return;
	act( "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_ABOUT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
	act( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
	act( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_WAIST );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
	&&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
	&&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	&&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;

	if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
	{
	    act( "$n wears $p around $s left wrist.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your left wrist.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_L );
	    return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
	{
	    act( "$n wears $p around $s right wrist.",
		ch, obj, NULL, TO_ROOM );
	    act( "You wear $p around your right wrist.",
		ch, obj, NULL, TO_CHAR );
	    equip_char( ch, obj, WEAR_WRIST_R );
	    return;
	}

	bug( "Wear_obj: no free wrist.", 0 );
	send_to_char( "You already wear two wrist items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;
	act( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
	act( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_SHIELD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
	    return;

	if ( get_obj_weight( obj ) > ( get_curr_str( ch ) / 2 ) )
	{
	    send_to_char( "It is too heavy for you to wield.\n\r", ch );
	    return;
	}

	act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
	act( "You wield $p.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_WIELD );
	return;
    }

    if( CAN_WEAR( obj, ITEM_HOLD ) )
    {
	if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	    return;
	act( "$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM );
	act( "You hold $p in your hands.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_HOLD );
	return;
    }

    if( fReplace )
	send_to_char( "You can't wear, wield, or hold that.\n\r", ch );

    return;
}


/*
 * Updated for multiple locations.
 * -Ant
 */
void do_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg1[ MAX_INPUT_LENGTH ];
    char	arg2[ MAX_INPUT_LENGTH ];
    long	location = NO_FLAG;

    one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if( arg1[0] == '\0' )
    {
	send_to_char_bw( "Wear, wield, or hold what?\n\r", ch );
	return;
    }

    if( arg2[0] == '\0' || !str_cmp( arg1, "all" ) )
    {
	if( !str_cmp( arg1, "all" ) )
	{
	    OBJ_DATA *obj_next;

	    for( obj = ch->carrying ; obj ; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if( obj->wear_loc == WEAR_NONE
		 && can_see_obj( ch, obj ) )
		    wear_obj( ch, obj, FALSE, location );
	    }
	    return;
	}
	else
	{
	    if( !( obj = get_obj_carry( ch, arg1 ) ) )
	    {
		send_to_char_bw( "You do not have that item.\n\r", ch );
		return;
	    }
	    wear_obj( ch, obj, TRUE, location );
	}
    }
    else
    {
	if( ( location = flag_value( wear_flags, arg2 ) ) == NO_FLAG )
	{
	    send_to_char_bw( "You can't wear anything there!\n\r", ch );
	    return;
	}
	else
	{
	    if( !( obj = get_obj_carry( ch, arg1 ) ) )
	    {
		send_to_char_bw( "You do not have that item.\n\r", ch );
		return;
	    }
	    if( !IS_SET( obj->wear_flags, location ) )
	    {
		send_to_char_bw( "You can't wear that there.\n\r", ch );
		return;
	    }
	    wear_obj( ch, obj, TRUE, location );
	}
    }

    return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    if(!str_cmp(arg,"all"))
       {
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	  {
            obj_next=obj->next_content;
            if( obj->wear_loc != WEAR_NONE) remove_obj(ch,obj->wear_loc,TRUE);
	  }
        }
   else 
    {

      if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
         {
	  send_to_char( "You do not have that item.\n\r", ch );
     	  return;
         }

    remove_obj( ch, obj->wear_loc, TRUE );
    }
    return;
}



void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
	act( "$n offers $mself to God, who graciously declines.",
	    ch, NULL, NULL, TO_ROOM );
	send_to_char(
	    "God appreciates your offer and may accept it later.", ch );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
	return;
    }

    send_to_char( "God gives you one gold coin for your sacrifice.\n\r", ch );
    ch->gold += 1;

    act( "$n sacrifices $p to God.", ch, obj, NULL, TO_ROOM );
    extract_obj( obj );
    return;
}



void do_quaff( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that potion.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
	send_to_char( "You can quaff only potions.\n\r", ch );
	return;
    }

    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR );

    obj_cast_spell( obj->potion->spell[0], obj->potion->level, ch, ch, NULL );
    obj_cast_spell( obj->potion->spell[1], obj->potion->level, ch, ch, NULL );
    obj_cast_spell( obj->potion->spell[2], obj->potion->level, ch, ch, NULL );
    obj_cast_spell( obj->potion->spell[3], obj->potion->level, ch, ch, NULL );

    extract_obj( obj );
    return;
}



void do_recite( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( scroll = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that scroll.\n\r", ch );
	return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can recite only scrolls.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
	victim = ch;
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
    act( "You recite $p.", ch, scroll, NULL, TO_CHAR );

    obj_cast_spell( scroll->scroll->spell[0], scroll->scroll->level, ch, victim, obj );
    obj_cast_spell( scroll->scroll->spell[1], scroll->scroll->level, ch, victim, obj );
    obj_cast_spell( scroll->scroll->spell[2], scroll->scroll->level, ch, victim, obj );
    obj_cast_spell( scroll->scroll->spell[3], scroll->scroll->level, ch, victim, obj );

    extract_obj( scroll );
    return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    int sn;

    if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }

    if( ( sn = staff->staff->spell ) < 0
     || sn >= MAX_SKILL
     || skill_table[sn].spell_fun == 0 )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if( staff->staff->charges > 0 )
    {
	act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
	act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
	for( vch = ch->in_room->people ; vch ; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;

	    switch( skill_table[sn].target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
		    continue;
		break;
		
	    case TAR_CHAR_DEFENSIVE:
		if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    }

	    obj_cast_spell( staff->staff->spell, staff->staff->level, ch, vch, NULL );
	}
    }

    if( --staff->staff->charges <= 0 )
    {
	act( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
	act( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
	extract_obj( staff );
    }

    return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }

    if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting != NULL )
	{
	    victim = ch->fighting;
	}
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if( wand->wand->charges > 0 )
    {
	if( victim )
	{
	    act( "$n zaps $N with $p.", ch, wand, victim, TO_ROOM );
	    act( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
	}
	else
	{
	    act( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
	    act( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
	}

	obj_cast_spell( wand->wand->spell, wand->wand->level, ch, victim, obj );
    }

    if( --wand->wand->charges <= 0 )
    {
	act( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
	act( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
	extract_obj( wand );
    }

    return;
}



void do_steal( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Steal what from whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_steal].beats );
    percent  = number_percent( ) + ( IS_AWAKE(victim) ? 10 : -50 );

    if( victim->position == POS_FIGHTING
     || !IS_NPC( victim )
     || ( !IS_NPC(ch) && percent > ch->pcdata->learned[gsn_steal] ) )
    {
	/*
	 * Failure.
	 */
	send_to_char( "Oops.\n\r", ch );
	act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
	sprintf( buf, "%s is a bloody thief!", ch->name );
	do_shout( victim, buf );
	if ( !IS_NPC(ch) )
	{
	    if ( IS_NPC(victim) )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	    else
	    {
		log_string( buf );
		if ( !IS_SET(ch->act, PLR_THIEF) )
		{
		    SET_BIT(ch->act, PLR_THIEF);
		    send_to_char( "*** You are now a THIEF!! ***\n\r", ch );
		    save_char_obj( ch );
		}
	    }
	}

	return;
    }

    if ( !str_cmp( arg1, "coin"  )
    ||   !str_cmp( arg1, "coins" )
    ||   !str_cmp( arg1, "gold"  ) )
    {
	int amount;

	amount = victim->gold * number_range(1, 10) / 100;
	if ( amount <= 0 )
	{
	    send_to_char( "You couldn't get any gold.\n\r", ch );
	    return;
	}

	ch->gold     += amount;
	victim->gold -= amount;
	sprintf( buf, "Bingo!  You got %d gold coins.\n\r", amount );
	send_to_char( buf, ch );
	return;
    }

    if ( ( obj = get_obj_carry( victim, arg1 ) ) == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }
	
    if( !can_drop_obj( ch, obj )
     || IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
    {
	send_to_char( "You can't pry it away.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	send_to_char( "You have your hands full.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
	if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
	    break;
    }

    if ( pShop == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_KILLER) )
    {
	do_say( keeper, "Killers are not welcome!" );
	sprintf( buf, "%s the KILLER is over here!\n\r", ch->name );
	do_shout( keeper, buf );
	return NULL;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_THIEF) )
    {
	do_say( keeper, "Thieves are not welcome!" );
	sprintf( buf, "%s the THIEF is over here!\n\r", ch->name );
	do_shout( keeper, buf );
	return NULL;
    }

    /*
     * Shop hours.
     */
    if ( time_info.hour < pShop->open_hour )
    {
	do_say( keeper, "Sorry, come back later." );
	return NULL;
    }
    
    if ( time_info.hour > pShop->close_hour )
    {
	do_say( keeper, "Sorry, come back tomorrow." );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	do_say( keeper, "I don't trade with folks I can't see." );
	return NULL;
    }

    return keeper;
}



int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost;

    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
	return 0;

    if ( fBuy )
    {
	cost = obj->cost * pShop->profit_buy  / 100;
    }
    else
    {
	OBJ_DATA *obj2;
	int itype;

	cost = 0;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	{
	    if ( obj->item_type == pShop->buy_type[itype] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }
	}

	for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj->pIndexData == obj2->pIndexData )
		cost /= 2;
	}
    }

    if( IS_SET( obj->item_type, ITEM_WAND ) )
	cost = cost * ( obj->wand->charges / obj->wand->max );

    if( IS_SET( obj->item_type, ITEM_STAFF ) )
	cost = cost * ( obj->staff->charges / obj->staff->max );

    return cost;
}



void do_buy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Buy what?\n\r", ch );
	return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
    {
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	ROOM_INDEX_DATA *in_room;

	if ( IS_NPC(ch) )
	    return;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	in_room     = ch->in_room;
	ch->in_room = pRoomIndexNext;
	pet         = get_char_room( ch, arg );
	ch->in_room = in_room;

	if( pet == NULL || !IS_SET(pet->act, ACT_PET) )
	{
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	if( IS_SET(ch->act, PLR_BOUGHT_PET) )
	{
	    send_to_char( "You already bought one pet this level.\n\r", ch );
	    return;
	}

	if( ch->gold < 1000 /* TODO */ )
	{
	    send_to_char( "You can't afford it.\n\r", ch );
	    return;
	}

	if( FALSE /* TODO */ )
	{
	    send_to_char( "You're not ready for this pet.\n\r", ch );
	    return;
	}

	ch->gold		-= 1000; /* TODO */
	pet			= create_mobile( pet->pIndexData );
	SET_BIT(ch->act, PLR_BOUGHT_PET);
	SET_BIT(pet->act, ACT_PET);
	SET_BIT(pet->affected_by, AFF_CHARM);

	argument = one_argument( argument, arg );
	if ( arg[0] != '\0' )
	{
	    sprintf( buf, "%s %s", pet->name, arg );
	    free_string( pet->name );
	    pet->name = str_dup( buf );
	}

	sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
	    pet->description, ch->name );
	free_string( pet->description );
	pet->description = str_dup( buf );

	char_to_room( pet, ch->in_room );
	add_follower( pet, ch );
	send_to_char( "Enjoy your pet.\n\r", ch );
	act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
	return;
    }
    else
    {
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	obj  = get_obj_carry( keeper, arg );
	cost = get_cost( keeper, obj, TRUE );

	if ( cost <= 0 || !can_see_obj( ch, obj ) )
	{
	    act( "$n tells you 'I don't sell that -- try 'list''.",
		keeper, NULL, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}

	if ( ch->gold < cost )
	{
	    act( "$n tells you 'You can't afford to buy $p'.",
		keeper, obj, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}
	
	if( FALSE /* TODO */ )
	{
	    act( "$n tells you 'You can't use $p yet'.",
		keeper, obj, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}

	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
	    send_to_char( "You can't carry that many items.\n\r", ch );
	    return;
	}

	if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "You can't carry that much weight.\n\r", ch );
	    return;
	}

	act( "$n buys $p.", ch, obj, NULL, TO_ROOM );
	act( "You buy $p.", ch, obj, NULL, TO_CHAR );
	ch->gold     -= cost;
	keeper->gold += cost;

	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    obj = create_object( obj->pIndexData );
	else
	    obj_from_char( obj );

	obj_to_char( obj, ch );
	return;
    }
}



void do_list( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];

    buf1[0] = '\0';

    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
    {
	ROOM_INDEX_DATA *pRoomIndexNext;
	CHAR_DATA *pet;
	bool found;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( pRoomIndexNext == NULL )
	{
	    bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	found = FALSE;
	for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
	{
	    if ( IS_SET(pet->act, ACT_PET) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    strcat( buf1, "Pets for sale:\n\r" );
		}
		sprintf( buf, "%8d - %s\n\r",
		    1000,
		    pet->short_descr ); /* TODO */
		strcat( buf1, buf );
	    }
	}
	if ( !found )
	    send_to_char( "Sorry, we're out of pets right now.\n\r", ch );

	send_to_char( buf1, ch );
	return;
    }
    else
    {
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;
	bool found;

	one_argument( argument, arg );

	if ( ( keeper = find_keeper( ch ) ) == NULL )
	    return;

	found = FALSE;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc == WEAR_NONE
	    &&   can_see_obj( ch, obj )
	    && ( cost = get_cost( keeper, obj, TRUE ) ) > 0
	    && ( arg[0] == '\0' || is_name( arg, obj->name ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    strcat( buf1, "[Price] Item\n\r" );
		}

		sprintf( buf, "[%5d] %s.\n\r",
		    cost, capitalize( obj->short_descr ) );
		strcat( buf1, buf );
	    }
	}

	if ( !found )
	{
	    if ( arg[0] == '\0' )
		send_to_char( "You can't buy anything here.\n\r", ch );
	    else
		send_to_char( "You can't buy that here.\n\r", ch );
	    return;
	}

	send_to_char( buf1, ch );
	return;
    }
}



void do_sell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sell what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    act( "$n sells $p.", ch, obj, NULL, TO_ROOM );
    sprintf( buf, "You sell $p for %d gold piece%s.",
	cost, cost == 1 ? "" : "s" );
    act( buf, ch, obj, NULL, TO_CHAR );
    ch->gold     += cost;
    keeper->gold -= cost;
    if ( keeper->gold < 0 )
	keeper->gold = 0;

    if ( obj->item_type == ITEM_TRASH )
    {
	extract_obj( obj );
    }
    else
    {
	obj_from_char( obj );
	obj_to_char( obj, keeper );
    }

    return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Value what?\n\r", ch );
	return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
	return;

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    sprintf( buf, "$n tells you 'I'll give you %d gold coins for $p'.", cost );
    act( buf, keeper, obj, ch, TO_VICT );
    ch->reply = keeper;

    return;
}
