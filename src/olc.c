/***************************************************************************
 *  File: olc.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
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
#include "olc.h"



/*
 * Local functions.
 */
AREA_DATA *get_area_data	args( ( int vnum ) );


/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool run_olc_editor( DESCRIPTOR_DATA *d )
{
    switch ( d->editor )
    {
    case ED_AREA:
	aedit( d->character, d->incomm );
	break;
    case ED_ROOM:
	redit( d->character, d->incomm );
	break;
    case ED_OBJECT:
	oedit( d->character, d->incomm );
	break;
    case ED_MOBILE:
	medit( d->character, d->incomm );
	break;
    default:
	return FALSE;
    }
    return TRUE;
}



char *olc_ed_name( CHAR_DATA *ch )
{
    static char buf[10];
    
    buf[0] = '\0';
    switch (ch->desc->editor)
    {
    case ED_AREA:
	sprintf( buf, "AEdit" );
	break;
    case ED_ROOM:
	sprintf( buf, "REdit" );
	break;
    case ED_OBJECT:
	sprintf( buf, "OEdit" );
	break;
    case ED_MOBILE:
	sprintf( buf, "MEdit" );
	break;
    default:
	sprintf( buf, " " );
	break;
    }
    return buf;
}



char *olc_ed_vnum( CHAR_DATA *ch )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMob;
    static char buf[10];
	
    buf[0] = '\0';
    switch ( ch->desc->editor )
    {
    case ED_AREA:
	pArea = (AREA_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pArea ? pArea->vnum : 0 );
	break;
    case ED_ROOM:
	pRoom = ch->in_room;
	sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
	break;
    case ED_OBJECT:
	pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
	break;
    case ED_MOBILE:
	pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
	sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
	break;
    default:
	sprintf( buf, " " );
	break;
    }

    return buf;
}



/*****************************************************************************
 Name:		show_olc_cmds
 Purpose:	Format up the commands from given table.
 Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void show_olc_cmds( CHAR_DATA *ch, const struct olc_cmd_type *olc_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  cmd;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    for (cmd = 0; olc_table[cmd].name[0] != '\0'; cmd++)
    {
	sprintf( buf, "%-15.15s", olc_table[cmd].name );
	strcat( buf1, buf );
	if ( ++col % 5 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 5 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:		show_commands
 Purpose:	Display all olc commands.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_commands( CHAR_DATA *ch, char *argument )
{
    switch (ch->desc->editor)
    {
	case ED_AREA:
	    show_olc_cmds( ch, aedit_table );
	    break;
	case ED_ROOM:
	    show_olc_cmds( ch, redit_table );
	    break;
	case ED_OBJECT:
	    show_olc_cmds( ch, oedit_table );
	    break;
	case ED_MOBILE:
	    show_olc_cmds( ch, medit_table );
	    break;
    }

    return FALSE;
}



/*****************************************************************************
 *                           Interpreter Tables.                             *
 *****************************************************************************/
const struct olc_cmd_type aedit_table[] =
{
/*  {   command		function		}, */

    {   "age",		aedit_age		},
    {   "builders",	aedit_builder		},
    {   "commands",	show_commands		},
    {   "create",	aedit_create		},
    {   "filename",	aedit_file		},
    {   "name",		aedit_name		},
    {   "recall",	aedit_recall		},
    {	"reset",	aedit_reset		},
    {   "security",	aedit_security		},
    {	"show",		aedit_show		},
    {   "vnum",		aedit_vnum		},
    {   "lvnum",	aedit_lvnum		},
    {   "uvnum",	aedit_uvnum		},

    {   "?",		show_help		},
    {   "version",	show_version		},

    {	"",		0,			}
};



const struct olc_cmd_type redit_table[] =
{
/*  {   command		function		}, */

    {   "commands",	show_commands		},
    {   "create",	redit_create		},
    {   "desc",		redit_desc		},
    {   "ed",		redit_ed		},
    {   "format",	redit_format		},
    {   "name",		redit_name		},
    {   "light",	redit_light		},
    {	"show",		redit_show		},

    {   "north",	redit_north		},
    {   "ne",		redit_northeast		},
    {   "nw",		redit_northwest		},
    {   "northeast",	redit_northeast		},
    {   "northwest",	redit_northwest		},
    {   "south",	redit_south		},
    {   "se",		redit_southeast		},
    {   "sw",		redit_southwest		},
    {   "southeast",	redit_southeast		},
    {   "southwest",	redit_southwest		},
    {   "east",		redit_east		},
    {   "west",		redit_west		},
    {   "up",		redit_up		},
    {   "down",		redit_down		},
    {   "walk",		redit_move		},

    /* New reset commands. */
    {	"mreset",	redit_mreset		},
    {	"oreset",	redit_oreset		},
    {	"mlist",	redit_mlist		},
    {	"olist",	redit_olist		},
    {	"mshow",	redit_mshow		},
    {	"oshow",	redit_oshow		},

    {   "?",		show_help		},
    {   "version",	show_version		},

    {	"",		0,			}
};



const struct olc_cmd_type oedit_table[] =
{
/*  {   command		function		}, */

    {   "addaffect",	oedit_addaffect		},
    {   "commands",	show_commands		},
    {   "cost",		oedit_cost		},
    {   "create",	oedit_create		},
    {   "delaffect",	oedit_delaffect		},
    {   "ed",		oedit_ed		},
    {   "long",		oedit_long		},
    {   "name",		oedit_name		},
    {   "requires",	oedit_requires		},
    {   "short",	oedit_short		},
    {	"show",		oedit_show		},
    {   "weight",	oedit_weight		},

    {   "?",		show_help		},
    {   "version",	show_version		},

    {	"",		0,			}
};



const struct olc_cmd_type medit_table[] =
{
/*  {   command		function		}, */

    {   "alignment",	medit_align		},
    {   "commands",	show_commands		},
    {   "create",	medit_create		},
    {   "desc",		medit_desc		},
    {   "exp",		medit_exp		},
    {   "long",		medit_long		},
    {   "name",		medit_name		},
    {   "shop",		medit_shop		},
    {   "short",	medit_short		},
    {	"show",		medit_show		},
    {   "spec",		medit_spec		},
    {   "race",		medit_race		},

    {   "?",		show_help		},
    {   "version",	show_version		},

    {	"",		0,			}
};
/*****************************************************************************
 *                          End Interpreter Tables.                          *
 *****************************************************************************/



/*****************************************************************************
 Name:		get_area_data
 Purpose:	Returns pointer to area with given vnum.
 Called by:	do_aedit(olc.c).
 ****************************************************************************/
AREA_DATA *get_area_data( int vnum )
{
    AREA_DATA *pArea;

    for (pArea = area_first; pArea; pArea = pArea->next )
    {
        if (pArea->vnum == vnum)
            return pArea;
    }

    return 0;
}



/*****************************************************************************
 Name:		edit_done
 Purpose:	Resets builder information on completion.
 Called by:	aedit, redit, oedit, medit(olc.c)
 ****************************************************************************/
bool edit_done( CHAR_DATA *ch )
{
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    return FALSE;
}



/*****************************************************************************
 *                              Interpreters.                                *
 *****************************************************************************/


/* Area Interpreter, called by do_aedit. */
void aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int  cmd;
    int  value;

    EDIT_AREA(ch, pArea);
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !IS_BUILDER( ch, pArea ) )
	send_to_char( "AEdit:  Insufficient security to modify area.\n\r", ch );

    if ( command[0] == '\0' )
    {
	aedit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg, FALSE );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *aedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, aedit_table[cmd].name ) )
	{
	    if ( (*aedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( area_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT(pArea->area_flags, value);

	send_to_char( "Flag toggled.\n\r", ch );
	return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg, FALSE );
    return;
}



/* Room Interpreter, called by do_redit. */
void redit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    AREA_DATA *pArea;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int  cmd;
    int  value;

    EDIT_ROOM(ch, pRoom);
    pArea = pRoom->area;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( !IS_BUILDER( ch, pArea ) )
        send_to_char( "REdit:  Insufficient security to modify room.\n\r", ch );

    if ( command[0] == '\0' )
    {
	redit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        interpret( ch, arg, FALSE );
        return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *redit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, redit_table[cmd].name ) )
	{
	    if ( (*redit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( room_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pRoom->room_flags, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room flag toggled.\n\r", ch );
        return;
    }

    if ( ( value = flag_value( sector_flags, arg ) ) != NO_FLAG )
    {
        pRoom->sector_type  = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sector type set.\n\r", ch );
        return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg, FALSE );
    return;
}



/* Object Interpreter, called by do_oedit. */
void oedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int  cmd;
    int  value;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_OBJ(ch, pObj);
    pArea = pObj->area;

    if ( !IS_BUILDER( ch, pArea ) )
	send_to_char( "OEdit: Insufficient security to modify area.\n\r", ch );

    if ( command[0] == '\0' )
    {
	oedit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg, FALSE );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *oedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, oedit_table[cmd].name ) )
	{
	    if ( (*oedit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( type_flags, arg ) ) != NO_FLAG )
    {
	if( !IS_SET( pObj->item_type, value ) )
	{
	    switch( value )
	    {
		case ITEM_LIGHT:
		    SET_BIT( pObj->item_type, ITEM_LIGHT );
		    if( !pObj->light )
			pObj->light = new_light( );
		    break;
		case ITEM_WAND:
		    SET_BIT( pObj->item_type, ITEM_WAND );
		    if( !pObj->wand )
			pObj->wand = new_wand( );
		    break;
		case ITEM_STAFF:
		    SET_BIT( pObj->item_type, ITEM_STAFF );
		    if( !pObj->staff )
			pObj->staff = new_staff( );
		    break;
        	case ITEM_SCROLL:
		    SET_BIT( pObj->item_type, ITEM_SCROLL );
		    if( !pObj->scroll )
			pObj->scroll = new_scroll( );
		    break;
        	case ITEM_POTION:
		    SET_BIT( pObj->item_type, ITEM_POTION );
		    if( !pObj->potion )
			pObj->potion = new_potion( );
		    break;
        	case ITEM_PILL:
		    SET_BIT( pObj->item_type, ITEM_PILL );
		    if( !pObj->pill )
			pObj->pill = new_pill( );
		    break;
        	case ITEM_WEAPON:
		    SET_BIT( pObj->item_type, ITEM_WEAPON );
		    if( !pObj->weapon )
			pObj->weapon = new_weapon( );
		    break;
        	case ITEM_ARMOUR:
		    SET_BIT( pObj->item_type, ITEM_ARMOUR );
		    if( !pObj->armour )
			pObj->armour = new_armour( );
		    break;
		case ITEM_CONTAINER:
		    SET_BIT( pObj->item_type, ITEM_CONTAINER );
		    if( !pObj->cont )
			pObj->cont = new_cont( );
		    break;
		case ITEM_DRINK_CON:
		    SET_BIT( pObj->item_type, ITEM_DRINK_CON );
		    if( !pObj->drink )
			pObj->drink = new_drink( );
		    break;
        	case ITEM_FOOD:
		    SET_BIT( pObj->item_type, ITEM_FOOD );
		    if( !pObj->food )
			pObj->food = new_food( );
		    break;
        	case ITEM_MONEY:
		    SET_BIT( pObj->item_type, ITEM_MONEY );
		    if( !pObj->money )
			pObj->money = new_money( );
		    break;
        	default:
		    SET_BIT( pObj->item_type, value );
		    break;
	    }
	}
	else if( IS_SET( pObj->item_type, value ) )
	{
	    switch( value )
	    {
		case ITEM_LIGHT:
		    REMOVE_BIT( pObj->item_type, ITEM_LIGHT );
		    free_light_index( pObj );
		    break;
		case ITEM_WAND:
		    REMOVE_BIT( pObj->item_type, ITEM_WAND );
		    free_wand_index( pObj );
		    break;
		case ITEM_STAFF:
		    REMOVE_BIT( pObj->item_type, ITEM_STAFF );
		    free_staff_index( pObj );
		    break;
        	case ITEM_SCROLL:
		    REMOVE_BIT( pObj->item_type, ITEM_SCROLL );
		    free_scroll_index( pObj );
		    break;
        	case ITEM_POTION:
		    REMOVE_BIT( pObj->item_type, ITEM_POTION );
		    free_potion_index( pObj );
		    break;
        	case ITEM_PILL:
		    REMOVE_BIT( pObj->item_type, ITEM_PILL );
		    free_pill_index( pObj );
		    break;
        	case ITEM_WEAPON:
		    REMOVE_BIT( pObj->item_type, ITEM_WEAPON );
		    free_weapon_index( pObj );
		    break;
        	case ITEM_ARMOUR:
		    REMOVE_BIT( pObj->item_type, ITEM_ARMOUR );
		    free_armour_index( pObj );
		    break;
		case ITEM_CONTAINER:
		    REMOVE_BIT( pObj->item_type, ITEM_CONTAINER );
		    free_cont_index( pObj );
		    break;
		case ITEM_DRINK_CON:
		    REMOVE_BIT( pObj->item_type, ITEM_DRINK_CON );
		    free_drink_index( pObj );
		    break;
        	case ITEM_FOOD:
		    REMOVE_BIT( pObj->item_type, ITEM_FOOD );
		    free_food_index( pObj );
		    break;
        	case ITEM_MONEY:
		    REMOVE_BIT( pObj->item_type, ITEM_MONEY );
		    free_money_index( pObj );
		    break;
        	default:
		    REMOVE_BIT( pObj->item_type, value );
		    break;
	    }
	}

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char_bw( "Type Changed.\n\r", ch);

        return;
    }

    if ( ( value = flag_value( extra_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pObj->extra_flags, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Extra flag toggled.\n\r", ch);
        return;
    }

    if ( ( value = flag_value( wear_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pObj->wear_flags, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Wear flag toggled.\n\r", ch);
        return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg, FALSE );
    return;
}



/* Mobile Interpreter, called by do_medit. */
void medit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int  cmd;
    int  value;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_MOB(ch, pMob);
    pArea = pMob->area;

    if ( !IS_BUILDER( ch, pArea ) )
	send_to_char( "MEdit: Insufficient security to modify area.\n\r", ch );

    if ( command[0] == '\0' )
    {
        medit_show( ch, argument );
        return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg, FALSE );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; *medit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, medit_table[cmd].name ) )
	{
	    if ( (*medit_table[cmd].olc_fun) ( ch, argument ) )
		SET_BIT( pArea->area_flags, AREA_CHANGED );
	    return;
	}
    }

    /* Take care of flags. */
    if ( ( value = flag_value( sex_flags, arg ) ) != NO_FLAG )
    {
        pMob->sex = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sex set.\n\r", ch);
        return;
    }


    if ( ( value = flag_value( act_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pMob->act, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Act flag toggled.\n\r", ch);
        return;
    }


    if ( ( value = flag_value( affect_flags, arg ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pMob->affected_by, value);

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Affect flag toggled.\n\r", ch);
        return;
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg, FALSE );
    return;
}



void do_aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );
    pArea = ch->in_room->area;

    if ( command[0] == 'r' && !str_prefix( command, "reset" ) )
    {
	if ( ch->desc->editor == ED_AREA )
	    reset_area( (AREA_DATA *)ch->desc->pEdit );
	else
	    reset_area( pArea );
	send_to_char( "Area reset.\n\r", ch );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( aedit_create( ch, argument ) )
	{
	    ch->desc->editor = ED_AREA;
	    pArea = (AREA_DATA *)ch->desc->pEdit;
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    aedit_show( ch, "" );
	}
	return;
    }

    if ( is_number( command ) )
    {
	if ( !( pArea = get_area_data( atoi(command) ) ) )
	{
	    send_to_char( "No such area vnum exists.\n\r", ch );
	    return;
	}
    }

    /*
     * Builder defaults to editing current area.
     */
    ch->desc->pEdit = (void *)pArea;
    ch->desc->editor = ED_AREA;
    aedit_show( ch, "" );
    return;
}



/* Entry point for editing room_index_data. */
void do_redit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );
    pRoom = ch->in_room;

    if ( command[0] == 'r' && !str_prefix( command, "reset" ) )
    {
	reset_room( pRoom );
	send_to_char( "Room reset.\n\r", ch );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( redit_create( ch, argument ) )
	{
	    char_from_room( ch );
	    char_to_room( ch, ch->desc->pEdit );
	    SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
    	}
    }

    /*
     * Builder defaults to editing current room.
     */
    ch->desc->editor = ED_ROOM;
    redit_show( ch, "" );
    return;
}



/* Entry point for editing obj_index_data. */
void do_oedit( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
	if ( !( pObj = get_obj_index( atoi( command ) ) ) )
	{
	    send_to_char( "OEdit:  That vnum does not exist.\n\r", ch );
	    return;
	}

	ch->desc->pEdit = (void *)pObj;
	ch->desc->editor = ED_OBJECT;
	oedit_show( ch, "" );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
        if ( oedit_create( ch, argument ) )
	{
	    pArea = get_vnum_area( atoi( argument ) );
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    ch->desc->editor = ED_OBJECT;
	    oedit_show( ch, "" );
	}
	return;
    }

    send_to_char( "OEdit:  There is no default object to edit.\n\r", ch );
    return;
}



/* Entry point for editing mob_index_data. */
void do_medit( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
	if ( !( pMob = get_mob_index( atoi( command ) ) ))
	{
	    send_to_char( "MEdit:  That vnum does not exist.\n\r", ch );
	    return;
	}

	ch->desc->pEdit = (void *)pMob;
	ch->desc->editor = ED_MOBILE;
	medit_show( ch, "" );
	return;
    }

    if ( command[0] == 'c' && !str_prefix( command, "create" ) )
    {
	if ( medit_create( ch, argument ) )
	{
	    pArea = get_vnum_area( atoi( argument ) );
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    ch->desc->editor = ED_MOBILE;
	    medit_show( ch, "" );
	}
	return;
    }

    send_to_char( "MEdit:  There is no default mobile to edit.\n\r", ch );
    return;
}



void display_resets( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA	*pRoom;
    RESET_DATA		*pReset;
    MOB_INDEX_DATA	*pMob = NULL;
    char 		buf   [ MAX_STRING_LENGTH ];
    char 		final [ MAX_STRING_LENGTH ];
    int 		iReset = 0;

    EDIT_ROOM(ch, pRoom);
    final[0]  = '\0';
    
    send_to_char ( 
  " No.  Loads    Description       Location         Vnum    Max  Description"
  "\n\r"
  "==== ======== ============= =================== ======== ===== ==========="
  "\n\r", ch );

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
	OBJ_INDEX_DATA  *pObj;
	MOB_INDEX_DATA  *pMobIndex;
	OBJ_INDEX_DATA  *pObjIndex;
	OBJ_INDEX_DATA  *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;

	final[0] = '\0';
	sprintf( final, "[%2d] ", ++iReset );

	switch ( pReset->command )
	{
	default:
	    sprintf( buf, "Bad reset command: %c.", pReset->command );
	    strcat( final, buf );
	    break;

	case 'M':
	    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }

            pMob = pMobIndex;
            sprintf( buf, "M[%5d] %-13.13s in room             R[%5d] [%3d] %-15.15s\n\r",
                       pReset->arg1, pMob->short_descr, pReset->arg3,
                       pReset->arg2, pRoomIndex->name );
            strcat( final, buf );

	    /*
	     * Check for pet shop.
	     * -------------------
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		if ( pRoomIndexPrev
		    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                    final[5] = 'P';
	    }

	    break;

	case 'O':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Object %d\n\r",
		    pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            pObj       = pObjIndex;

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }

            sprintf( buf, "O[%5d] %-13.13s in room             "
                          "R[%5d]       %-15.15s\n\r",
                          pReset->arg1, pObj->short_descr,
                          pReset->arg3, pRoomIndex->name );
            strcat( final, buf );

	    break;

	case 'P':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Put Object - Bad Object %d\n\r",
                    pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            pObj       = pObjIndex;

	    if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Put Object - Bad To Object %d\n\r",
                    pReset->arg3 );
                strcat( final, buf );
                continue;
	    }

	    sprintf( buf,
		"O[%5d] %-13.13s inside              O[%5d]       %-15.15s\n\r",
		pReset->arg1,
		pObj->short_descr,
		pReset->arg3,
		pObjToIndex->short_descr );
            strcat( final, buf );

	    break;

	case 'G':
	case 'E':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Give/Equip Object - Bad Object %d\n\r",
                    pReset->arg1 );
                strcat( final, buf );
                continue;
	    }

            pObj       = pObjIndex;

	    if ( !pMob )
	    {
                sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
                strcat( final, buf );
                break;
	    }

	    if ( pMob->pShop )
	    {
	    sprintf( buf,
		"O[%5d] %-13.13s in the inventory of S[%5d]       %-15.15s\n\r",
		pReset->arg1,
		pObj->short_descr,                           
		pMob->vnum,
		pMob->short_descr  );
	    }
	    else
	    sprintf( buf,
		"O[%5d] %-13.13s %-19.19s M[%5d]       %-15.15s\n\r",
		pReset->arg1,
		pObj->short_descr,
		(pReset->command == 'G') ?
		    flag_string( wear_loc_strings, WEAR_NONE )
		  : flag_string( wear_loc_strings, pReset->arg3 ),
		  pMob->vnum,
		  pMob->short_descr );
	    strcat( final, buf );

	    break;

	/*
	 * Doors are set in rs_flags don't need to be displayed.
	 * If you want to display them then uncomment the new_reset
	 * line in the case 'D' in load_resets in db.c and here.
	 *
	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );
	    sprintf( buf, "R[%5d] %s door of %-19.19s reset to %s\n\r",
		pReset->arg1,
		capitalize( dir_name[ pReset->arg2 ] ),
		pRoomIndex->name,
		flag_string( door_resets, pReset->arg3 ) );
	    strcat( final, buf );

	    break;
	 *
	 * End Doors Comment.
	 */
	case 'R':
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
		sprintf( buf, "Randomize Exits - Bad Room %d\n\r",
		    pReset->arg1 );
		strcat( final, buf );
		continue;
	    }

	    sprintf( buf, "R[%5d] Exits are randomized in %s\n\r",
		pReset->arg1, pRoomIndex->name );
	    strcat( final, buf );

	    break;
	}
	send_to_char( final, ch );
    }

    return;
}



/*****************************************************************************
 Name:		add_reset
 Purpose:	Inserts a new reset in the given index slot.
 Called by:	do_resets(olc.c).
 ****************************************************************************/
void add_reset( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index )
{
    RESET_DATA *reset;
    int iReset = 0;

    if ( !room->reset_first )
    {
	room->reset_first	= pReset;
	room->reset_last	= pReset;
	pReset->next		= NULL;
	return;
    }

    index--;

    if ( index == 0 )	/* First slot (1) selected. */
    {
	pReset->next = room->reset_first;
	room->reset_first = pReset;
	return;
    }

    /*
     * If negative slot( <= 0 selected) then this will find the last.
     */
    for ( reset = room->reset_first; reset->next; reset = reset->next )
    {
	if ( ++iReset == index )
	    break;
    }

    pReset->next	= reset->next;
    reset->next		= pReset;
    if ( !pReset->next )
	room->reset_last = pReset;
    return;
}



void do_resets( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char arg5[MAX_INPUT_LENGTH];
    RESET_DATA *pReset = NULL;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );

    /*
     * Display resets in current room.
     * -------------------------------
     */
    if ( arg1[0] == '\0' )
    {
	if ( ch->in_room->reset_first )
	{
	    send_to_char(
		"Resets: M = mobile, R = room, O = object, "
		"P = pet, S = shopkeeper\n\r", ch );
	    display_resets( ch );
	}
	else
	    send_to_char( "No resets in this room.\n\r", ch );
    }

    if ( !IS_BUILDER( ch, ch->in_room->area ) )
    {
	send_to_char( "Resets: Invalid security for editing this area.\n\r",
                      ch );
	return;
    }

    /*
     * Take index number and search for commands.
     * ------------------------------------------
     */
    if ( is_number( arg1 ) )
    {
	ROOM_INDEX_DATA *pRoom = ch->in_room;

	/*
	 * Delete a reset.
	 * ---------------
	 */
	if ( !str_cmp( arg2, "delete" ) )
	{
	    int insert_loc = atoi( arg1 );

	    if ( !ch->in_room->reset_first )
	    {
		send_to_char( "No resets in this area.\n\r", ch );
		return;
	    }

	    if ( insert_loc-1 <= 0 )
	    {
		pReset = pRoom->reset_first;
		pRoom->reset_first = pRoom->reset_first->next;
		if ( !pRoom->reset_first )
		    pRoom->reset_last = NULL;
	    }
	    else
	    {
		int iReset = 0;
		RESET_DATA *prev = NULL;

		for ( pReset = pRoom->reset_first;
		  pReset;
		  pReset = pReset->next )
		{
		    if ( ++iReset == insert_loc )
			break;
		    prev = pReset;
		}

		if ( !pReset )
		{
		    send_to_char( "Reset not found.\n\r", ch );
		    return;
		}

		if ( prev )
		    prev->next = prev->next->next;
		else
		    pRoom->reset_first = pRoom->reset_first->next;

		for ( pRoom->reset_last = pRoom->reset_first;
		  pRoom->reset_last->next;
		  pRoom->reset_last = pRoom->reset_last->next );
	    }

	    free_reset_data( pReset );
	    send_to_char( "Reset deleted.\n\r", ch );
	}
	else
	/*
	 * Add a reset.
	 * ------------
	 */
	if ( (!str_cmp( arg2, "mob" ) && is_number( arg3 ))
	  || (!str_cmp( arg2, "obj" ) && is_number( arg3 )) )
	{
	    /*
	     * Check for Mobile reset.
	     * -----------------------
	     */
	    if ( !str_cmp( arg2, "mob" ) )
	    {
		pReset = new_reset_data();
		pReset->command = 'M';
		pReset->arg1    = atoi( arg3 );
		pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 1; /* Max # */
		pReset->arg3    = ch->in_room->vnum;
	    }
	    else
	    /*
	     * Check for Object reset.
	     * -----------------------
	     */
	    if ( !str_cmp( arg2, "obj" ) )
	    {
		pReset = new_reset_data();
		pReset->arg1    = atoi( arg3 );
		/*
		 * Inside another object.
		 * ----------------------
		 */
		if ( !str_prefix( arg4, "inside" ) )
		{
		    pReset->command = 'P';
		    pReset->arg2    = 0;
		    pReset->arg3    = is_number( arg5 ) ? atoi( arg5 ) : 1;
		}
		else
		/*
		 * Inside the room.
		 * ----------------
		 */
		if ( !str_cmp( arg4, "room" ) )
		{
		    pReset = new_reset_data();
		    pReset->command = 'O';
		    pReset->arg2     = 0;
		    pReset->arg3     = ch->in_room->vnum;
		}
		else
		/*
		 * Into a Mobile's inventory.
		 * --------------------------
		 */
		{
		    if ( flag_value( wear_loc_flags, arg4 ) == NO_FLAG )
		    {
			send_to_char( "Resets: '? wear-loc'\n\r", ch );
			return;
		    }
		    pReset = new_reset_data();
		    pReset->arg3 = flag_value( wear_loc_flags, arg4 );
		    if ( pReset->arg2 == WEAR_NONE )
			pReset->command = 'G';
		    else
			pReset->command = 'E';
		}
	    }

	    add_reset( ch->in_room, pReset, atoi( arg1 ) );
	    send_to_char( "Reset added.\n\r", ch );
	}
	else
	{
	send_to_char( "Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch );
	send_to_char( "        RESET <number> OBJ <vnum> in <vnum>\n\r", ch );
	send_to_char( "        RESET <number> OBJ <vnum> room\n\r", ch );
	send_to_char( "        RESET <number> MOB <vnum> [<max #>]\n\r", ch );
	send_to_char( "        RESET <number> DELETE\n\r", ch );
	}
    }

    return;
}



/*****************************************************************************
 Name:		do_alist
 Purpose:	Normal command to list areas and display area information.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_alist( CHAR_DATA *ch, char *argument )
{
    char buf    [ MAX_STRING_LENGTH ];
    char result [ MAX_STRING_LENGTH*2 ];	/* May need tweaking. */
    AREA_DATA *pArea;

    sprintf( result, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
       "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	sprintf( buf, "[%3d] %-29.29s (%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
	     pArea->vnum,
	     &pArea->name[8],
	     pArea->lvnum,
	     pArea->uvnum,
	     pArea->filename,
	     pArea->security,
	     pArea->builders );
	     strcat( result, buf );
    }

    send_to_char( result, ch );
    return;
}
