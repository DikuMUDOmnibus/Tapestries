/***************************************************************************
 *  File: olc_save.c                                                       *
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
/* OLC_SAVE.C
 * This takes care of saving all the .are information.
 * Notes:
 * -If a good syntax checker is used for setting vnum ranges of areas
 *  then it would become possible to just cycle through vnums instead
 *  of using the iHash stuff and checking that the room or reset or
 *  mob etc is part of that area.
 */

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



/*****************************************************************************
 Name:		fix_string
 Purpose:	Returns a string without \r and ~.
 ****************************************************************************/
char *fix_string( const char *str )
{
    static	char	strfix[ MAX_STRING_LENGTH ];
    		int	i;
    		int	o;

    if( !str )
        return '\0';

    for( o = i = 0 ; str[i+o] != '\0' ; i++ )
    {
        if( str[ i + o ] == '\r' || str[ i + o ] == '~' )
            o++;
        strfix[ i ] = str[ i + o ];
    }
    strfix[ i ] = '\0';

    return strfix;
}



/*****************************************************************************
 Name:		save_area_list
 Purpose:	Saves the listing of files to be loaded at startup.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area_list()
{
    AREA_DATA	*pArea;
    FILE	*fp;

    if( !( fp = fopen( "area.lst", "w" ) ) )
    {
	bug( "Save_area_list: fopen", 0 );
	perror( "area.lst" );
    }
    else
    {
	/*
	 * Add any help files that need to be loaded at
	 * startup to this section.
	 */
	fprintf( fp, "main.hlp\n" );
	fprintf( fp, "social.soc\n" );

	for( pArea = area_first ; pArea ; pArea = pArea->next )
	{
	    fprintf( fp, "%s\n", pArea->filename );
	}

	fprintf( fp, "$\n" );
	fclose( fp );
    }

    return;
}



/*****************************************************************************
 Name:		save_mobiles
 Purpose:	Save #MOBILES secion of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA	*pMobIndex;
    int			vnum;

    fprintf( fp, "#MOBILES\n" );
    for( vnum = pArea->lvnum ; vnum <= pArea->uvnum ; vnum++ )
    {
	if( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if( pMobIndex->area == pArea )
            {
                fprintf( fp, "#%d\n",		pMobIndex->vnum );
                fprintf( fp, "%s~\n",		pMobIndex->player_name );
                fprintf( fp, "%s~\n",		pMobIndex->short_descr );
                fprintf( fp, "%s~\n",		fix_string( pMobIndex->long_descr ) );
                fprintf( fp, "%s~\n",		fix_string( pMobIndex->description ) );
		if( pMobIndex->race )
		    fprintf( fp, "'%s' ",	
			pMobIndex->race->name ? pMobIndex->race->name : "unique" );
		else
		    fprintf( fp, "'unique' " );	
                fprintf( fp, "%ld ",		pMobIndex->act );
                fprintf( fp, "%ld ",		pMobIndex->affected_by );
                fprintf( fp, "%d S\n",		pMobIndex->alignment );
                fprintf( fp, "%d ",		pMobIndex->hitroll );
                fprintf( fp, "%d ",		pMobIndex->ac );
                fprintf( fp, "%dd%d+%d ",	pMobIndex->hitnodice, 
                                                pMobIndex->hitsizedice, 
                                                pMobIndex->hitplus );
                fprintf( fp, "%dd%d+%d\n",	pMobIndex->damnodice, 
                                                pMobIndex->damsizedice, 
                                                pMobIndex->damplus );
                fprintf( fp, "%d ",		pMobIndex->gold );
		fprintf( fp, "%d\n0 0 ",	pMobIndex->exp );
                fprintf( fp, "%d\n",		pMobIndex->sex );

		mprog_write_programs( fp, pMobIndex );
            }
        }
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}



/*****************************************************************************
 Name:		save_objects
 Purpose:	Save #OBJECTS section of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_objects( FILE *fp, AREA_DATA *pArea )
{
    EXTRA_DESCR_DATA	*pEd;
    OBJ_INDEX_DATA	*pObjIndex;
    AFFECT_DATA		*pAf;
    int			vnum;
    int			tag;

    fprintf( fp, "#OBJECTS\n" );

    for( vnum = pArea->lvnum ; vnum <= pArea->uvnum ; vnum++ )
    {
	if( ( pObjIndex = get_obj_index(vnum) ) )
	{
	    if( pObjIndex->area == pArea )
            {
                fprintf( fp, "#%d\n",    pObjIndex->vnum );
                fprintf( fp, "%s~\n",    pObjIndex->name );
                fprintf( fp, "%s~\n",    pObjIndex->short_descr );
                fprintf( fp, "%s~\n",    fix_string( pObjIndex->description ) );
                fprintf( fp, "~\n" );
                fprintf( fp, "%ld ",      pObjIndex->item_type );
                fprintf( fp, "%ld ",      pObjIndex->extra_flags );
                fprintf( fp, "%ld\n",     pObjIndex->wear_flags);
                fprintf( fp, "%d ",	pObjIndex->weight );
                fprintf( fp, "%d\n",	pObjIndex->cost );
                fprintf( fp, "%hd %hd %hd %hd %hd\n",
		    pObjIndex->requires[ 0 ],
		    pObjIndex->requires[ 1 ],
		    pObjIndex->requires[ 2 ],
		    pObjIndex->requires[ 3 ],
		    pObjIndex->requires[ 4 ] );

                for( pAf = pObjIndex->affected ; pAf ; pAf = pAf->next )
                {
		    fprintf( fp, "A\n%d %d\n",  pAf->location, pAf->modifier );
                }

                for( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
                {
		    fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
			fix_string( pEd->description ) );
                }

		if( pObjIndex->weapon
		 && IS_SET( pObjIndex->item_type, ITEM_WEAPON ) )
		{
		    fprintf( fp, "OA %d '%s' %ld %d %dd%d+%d\n",
			pObjIndex->weapon->type,
			flag_string( weapon_flags, pObjIndex->weapon->message ),
			pObjIndex->weapon->flags,
			pObjIndex->weapon->hit,
			pObjIndex->weapon->damage[0],
			pObjIndex->weapon->damage[1],
			pObjIndex->weapon->damage[2] );
		}

		if( pObjIndex->armour
		 && IS_SET( pObjIndex->item_type, ITEM_ARMOUR ) )
		{
		    fprintf( fp, "OB %d %d %d %d\n",
			pObjIndex->armour->ac[0],
			pObjIndex->armour->ac[1],
			pObjIndex->armour->ac[2],
			pObjIndex->armour->ac[3] );
		}

		if( pObjIndex->cont
		 && IS_SET( pObjIndex->item_type, ITEM_CONTAINER ) )
		{
		    fprintf( fp, "OC %ld %d %d %d\n",
			pObjIndex->cont->closed,
			pObjIndex->cont->key,
			pObjIndex->cont->capacity,
			pObjIndex->cont->single );
		}

		if( pObjIndex->drink
		 && IS_SET( pObjIndex->item_type, ITEM_DRINK_CON ) )
		{
		    fprintf( fp, "OD %d %d %d %d\n",
			pObjIndex->drink->volume,
			pObjIndex->drink->limit,
			pObjIndex->drink->liquid,
			pObjIndex->drink->poison );
		}

		if( pObjIndex->food
		 && IS_SET( pObjIndex->item_type, ITEM_FOOD ) )
		{
		    fprintf( fp, "OE %d %d\n",
			pObjIndex->food->bite,
			pObjIndex->food->poison ? 1 : 0 );
		}

		if( pObjIndex->potion
		 && IS_SET( pObjIndex->item_type, ITEM_POTION ) )
		{
		    fprintf( fp, "OF %d",
			pObjIndex->potion->level );

		    for( tag = 0 ; tag < 4 ; tag++ )
		    {
			if( pObjIndex->potion->spell[ tag ] == -1 )
			{
			    fprintf( fp, " ''" );
			}
			else
			{
			    fprintf( fp, " '%s'",
				skill_table[pObjIndex->potion->spell[ tag ]].name );
			}
		    }
		    fprintf( fp, "\n" );
		}

		if( pObjIndex->pill
		 && IS_SET( pObjIndex->item_type, ITEM_PILL ) )
		{
		    fprintf( fp, "OG %d",
			pObjIndex->pill->level );

		    for( tag = 0 ; tag < 4 ; tag++ )
		    {
			if( pObjIndex->pill->spell[ tag ] == -1 )
			{
			    fprintf( fp, " ''" );
			}
			else
			{
			    fprintf( fp, " '%s'",
				skill_table[pObjIndex->pill->spell[ tag ]].name );
			}
		    }
		    fprintf( fp, "\n" );
		}

		if( pObjIndex->scroll
		 && IS_SET( pObjIndex->item_type, ITEM_SCROLL ) )
		{
		    fprintf( fp, "OH %d",
			pObjIndex->scroll->level );

		    for( tag = 0 ; tag < 4 ; tag++ )
		    {
			if( pObjIndex->scroll->spell[ tag ] == -1 )
			{
			    fprintf( fp, " ''" );
			}
			else
			{
			    fprintf( fp, " '%s'",
				skill_table[pObjIndex->scroll->spell[ tag ]].name );
			}
		    }
		    fprintf( fp, "\n" );
		}

		if( pObjIndex->staff
		 && IS_SET( pObjIndex->item_type, ITEM_STAFF ) )
		{
		    fprintf( fp, "OI %d %d %d '%s'\n",
			pObjIndex->staff->level,
			pObjIndex->staff->charges,
			pObjIndex->staff->max,
			pObjIndex->staff->spell != -1 ?
			  skill_table[pObjIndex->staff->spell].name : "" );
		}

		if( pObjIndex->wand
		 && IS_SET( pObjIndex->item_type, ITEM_WAND ) )
		{
		    fprintf( fp, "OJ %d %d %d '%s'\n",
			pObjIndex->wand->level,
			pObjIndex->wand->charges,
			pObjIndex->wand->max,
			pObjIndex->wand->spell != -1 ?
			  skill_table[pObjIndex->wand->spell].name : "" );
		}

		if( pObjIndex->light
		 && IS_SET( pObjIndex->item_type, ITEM_LIGHT ) )
		{
		    fprintf( fp, "OK %d %d\n",
			pObjIndex->light->duration,
			pObjIndex->light->intensity );
		}

		if( pObjIndex->money
		 && IS_SET( pObjIndex->item_type, ITEM_MONEY ) )
		{
		    fprintf( fp, "OL %d %d %d\n",
			pObjIndex->money->gold,
			pObjIndex->money->silver,
			pObjIndex->money->copper );
		}
            }
        }
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}



/* OLC 1.1b */
/*****************************************************************************
 Name:		save_rooms
 Purpose:	Save #ROOMDATA section of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_rooms( FILE *fp, AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA *pExit;
    int vnum;
    int door;

    fprintf( fp, "#ROOMDATA\n" );
    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pRoomIndex = get_room_index(vnum) ) )
	{
	    if ( pRoomIndex->area == pArea )
	    {
		fprintf( fp, "#%d\n", pRoomIndex->vnum );
		fprintf( fp, "%s~\n", pRoomIndex->name );
		fprintf( fp, "%s~\n", fix_string( pRoomIndex->description ) );
		fprintf( fp, "%s ",   ascii_flag( pRoomIndex->room_flags ) );
		fprintf( fp, "%d ",   pRoomIndex->sector_type );
		fprintf( fp, "%d\n",  pRoomIndex->light );

		for ( pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next )
                {
		    fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
			fix_string( pEd->description ) );
                }

		for( door = 0; door < MAX_DIR; door++ )
		{
		    if ( ( pExit = pRoomIndex->exit[door] ) )
		    {
			fprintf( fp, "D%d\n", door );
			fprintf( fp, "%s~\n",
			    fix_string( pExit->description ) );
			fprintf( fp, "%s~\n", pExit->keyword );
			fprintf( fp, "%s %d %d\n", ascii_flag( pExit->rs_flags ),
			    pExit->key,
			    pExit->to_room ? pExit->to_room->vnum : 0 );
		    }
		}
		fprintf( fp, "S\n" );
	    }
	}
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}


/* OLC 1.1b */
/*****************************************************************************
 Name:		save_specials
 Purpose:	Save #SPECIALS section of area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_specials( FILE *fp, AREA_DATA *pArea )
{
    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    
    fprintf( fp, "#SPECIALS\n" );

    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pMobIndex = get_mob_index(vnum) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->spec_fun )
            {
                fprintf( fp, "M %d %s\n", pMobIndex->vnum,
                                          spec_string( pMobIndex->spec_fun ) );
            }
        }
    }

    fprintf( fp, "S\n\n\n\n" );
    return;
}


/* OLC 1.1b */
/*****************************************************************************
 Name:		vsave_specials
 Purpose:	Save #SPECIALS section of area file.
 		New formating thanks to Rac.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void vsave_specials( FILE *fp, AREA_DATA *pArea )
{
    int vnum;
    MOB_INDEX_DATA *pMobIndex;
    
    fprintf( fp, "#SPECIALS\n" );

    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pMobIndex = get_mob_index(vnum) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->spec_fun )
            {
                fprintf( fp, "M %d %s \t; %s\n", pMobIndex->vnum,
                                                 spec_string( pMobIndex->spec_fun ),
                                                 pMobIndex->short_descr );
            }
        }
    }

    fprintf( fp, "S\n\n\n\n" );
    return;
}



/* OLC 1.1b */
/*****************************************************************************
 Name:		save_resets
 Purpose:	Saves the #RESETS section of an area file.
                New format thanks to Rac.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_resets( FILE *fp, AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[MAX_STRING_LENGTH];
    int vnum;

    fprintf( fp, "#RESETS\n" );

    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pRoomIndex = get_room_index(vnum) ) )
	{
	    if ( pRoomIndex->area == pArea )
	    {
    for ( pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next )
    {
	switch ( pReset->command )
	{
	default:
	    bug( "Save_resets: bad command %c.", pReset->command );
	    break;

	case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M 0 %d %d %d\n", 
	        pReset->arg1,
                pReset->arg2,
                pReset->arg3 );
            break;

	case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "O 0 %d 0 %d\n", 
	        pReset->arg1,
                pReset->arg3 );
            break;

	case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "P 0 %d 0 %d\n", 
	        pReset->arg1,
                pReset->arg3  );
            break;

	case 'G':
	    fprintf( fp, "G 0 %d 0\n", pReset->arg1 );
            if ( !pLastMob )
            {
                sprintf( buf,
                    "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'E':
	    fprintf( fp, "E 0 %d 0 %d\n",
	        pReset->arg1,
                pReset->arg3 );
            if ( !pLastMob )
            {
                sprintf( buf,
                    "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'D':
            break;

	case 'R':
	    fprintf( fp, "R 0 %d %d\n", 
	        pReset->arg1,
                pReset->arg2 );
            break;
        }	/* End switch */
    }	/* End for pReset */
	    }	/* End if correct area */
	}	/* End if pRoomIndex */
    }	/* End for vnum */
    fprintf( fp, "S\n\n\n\n" );
    return;
}



/* OLC 1.1b */
/*****************************************************************************
 Name:		save_resets
 Purpose:	Saves the #RESETS section of an area file.
                New format thanks to Rac.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void vsave_resets( FILE *fp, AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[MAX_STRING_LENGTH];
    int vnum;

    fprintf( fp, "#RESETS\n" );

    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pRoomIndex = get_room_index(vnum) ) )
	{
	    if ( pRoomIndex->area == pArea )
	    {
    for ( pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next )
    {
	switch ( pReset->command )
	{
	default:
	    bug( "Save_resets: bad command %c.", pReset->command );
	    break;

	case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M 0 %d %2d %-5d \t; %s to %s\n", 
	        pReset->arg1,
                pReset->arg2,
                pReset->arg3,
                pLastMob->short_descr,
                pRoomIndex->name );
            break;

	case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "O 0 %d  0 %-5d \t; %s to %s\n", 
	        pReset->arg1,
                pReset->arg3,
                capitalize(pLastObj->short_descr),
                pRoomIndex->name );
            break;

	case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "P 0 %d  0 %-5d \t; %s inside %s\n", 
	        pReset->arg1,
                pReset->arg3,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastObj ? pLastObj->short_descr : "!NO_OBJ!" );

            if ( !pLastObj )	/* Thanks Rac! */
            {
                sprintf( buf, "Save_resets: P with !NO_OBJ! in [%s]", pArea->filename );
                bug( buf, 0 );
            }

            break;

	case 'G':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "G 0 %d  0      \t;   %s\n",
	        pReset->arg1,
	        capitalize(pLastObj->short_descr) );

            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'E':
	    fprintf( fp, "E 0 %d  0 %-5d \t;   %s %s\n",
	        pReset->arg1,
                pReset->arg3,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                flag_string( wear_loc_strings, pReset->arg3 ) );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
                bug( buf, 0 );
            }
            break;

	case 'D':
            break;

	case 'R':
	    fprintf( fp, "R 0 %d %2d      \t; Randomize %s\n", 
	        pReset->arg1,
                pReset->arg2,
                pRoomIndex->name );
            break;
        }	/* End switch */
    }	/* End for pReset */
	    }	/* End if correct area */
	}	/* End if pRoomIndex */
    }	/* End for vnum */
    fprintf( fp, "S\n\n\n\n" );
    return;
}



/* OLC 1.1b */
/*****************************************************************************
 Name:		save_shops
 Purpose:	Saves the #SHOPS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_shops( FILE *fp, AREA_DATA *pArea )
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;
    int vnum;
    
    fprintf( fp, "#SHOPS\n" );

    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pMobIndex = get_mob_index(vnum) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->pShop )
            {
                pShopIndex = pMobIndex->pShop;

                fprintf( fp, "%d ", pShopIndex->keeper );
                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                {
                    if ( pShopIndex->buy_type[iTrade] != 0 )
                    {
                       fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
                    }
                    else
                       fprintf( fp, "0 ");
                }
                fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
                fprintf( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}



/* OLC 1.1b */
/*****************************************************************************
 Name:		vsave_shops
 Purpose:	Saves the #SHOPS section of an area file.
                New formating thanks to Rac.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void vsave_shops( FILE *fp, AREA_DATA *pArea )
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;
    int vnum;
    
    fprintf( fp, "#SHOPS\n" );

    for( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if( ( pMobIndex = get_mob_index(vnum) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->pShop )
            {
                pShopIndex = pMobIndex->pShop;

                fprintf( fp, "%d ", pShopIndex->keeper );
                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                {
                    if ( pShopIndex->buy_type[iTrade] != 0 )
                    {
                       fprintf( fp, "%2d ", pShopIndex->buy_type[iTrade] );
                    }
                    else
                       fprintf( fp, " 0 ");
                }
                fprintf( fp, "%3d %3d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
                fprintf( fp, "%2d %2d ", pShopIndex->open_hour, pShopIndex->close_hour );
                fprintf( fp, "\t; %s\n", get_mob_index( pShopIndex->keeper )->short_descr );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}


/*****************************************************************************
 Name:		save_area
 Purpose:	Save an area, note that this format is new.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area( AREA_DATA *pArea )
{
    FILE	*fp;
    char	buf[      MAX_STRING_LENGTH ];
    char	file[     MAX_STRING_LENGTH ];
    char	safefile[ MAX_STRING_LENGTH ];

    sprintf( file, "%s%s", CURRENT_DIR, pArea->filename );
    sprintf( safefile, "%s%s", AREA_TMP_DIR, pArea->filename );

    fclose( fpReserve );
    if( !( fp = fopen( safefile, "w" ) ) )
    {
	bug( "Open_area: fopen", 0 );
	perror( safefile );
    }

    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Name        %s~\n",        pArea->name );
    fprintf( fp, "Builders    %s~\n",        fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs       %d %d\n",      pArea->lvnum, pArea->uvnum );
    fprintf( fp, "Security    %d\n",         pArea->security );
    fprintf( fp, "Recall      %d\n",         pArea->recall );
    fprintf( fp, "End\n\n\n\n" );

    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );

    if ( IS_SET(pArea->area_flags, AREA_VERBOSE) )	/* OLC 1.1b */
    {
	vsave_specials( fp, pArea );
	vsave_resets( fp, pArea );
	vsave_shops( fp, pArea );
    }
    else
    {
	save_specials( fp, pArea );
	save_resets( fp, pArea );
	save_shops( fp, pArea );
    }

    fprintf( fp, "#$\n" );

    fclose( fp );

    /*
     * Copy the temporary file into the real file directory.
     *   Note: cp -bf means backup and no prompt.
     */
    sprintf( buf, "cp -bf %s %s", safefile, file );
    system( buf );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/* OLC 1.1b */
/*****************************************************************************
 Name:		do_asave
 Purpose:	Entry point for saving area data.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_asave( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    AREA_DATA *pArea;
    int value;

    if ( !ch )       /* Do an autosave */
    {
	send_to_all_char( "Please wait for the database to be saved.\n\r" );
	save_area_list();
	for( pArea = area_first; pArea; pArea = pArea->next )
	{
	    save_area( pArea );
	    REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED );
	}
	send_to_all_char( "Thank you, the database has been saved.\n\r" );
	return;
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
    send_to_char( "Syntax:\n\r", ch );
    send_to_char( "  asave <vnum>    - saves a particular area\n\r",	 ch );
    send_to_char( "  asave list      - saves the area.lst file\n\r",	 ch );
    send_to_char( "  asave area      - saves the area being edited\n\r", ch );
    send_to_char( "  asave changed   - saves all changed zones\n\r",	 ch );
    send_to_char( "  asave world     - saves the world! (db dump)\n\r",	 ch );
    send_to_char( "  asave ^ verbose - saves in verbose mode\n\r", ch );
    send_to_char( "\n\r", ch );
        return;
    }

    /* Snarf the value (which need not be numeric). */
    value = atoi( arg1 );

    /* Save the area of given vnum. */
    /* ---------------------------- */

    if ( !( pArea = get_area_data( value ) ) && is_number( arg1 ) )
    {
	send_to_char( "That area does not exist.\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "You are not a builder for this area.\n\r", ch );
	    return;
	}

	save_area_list();
	if ( !str_cmp( "verbose", argument ) )
	    SET_BIT( pArea->area_flags, AREA_VERBOSE );
	save_area( pArea );
	REMOVE_BIT( pArea->area_flags, AREA_VERBOSE );
	return;
    }

    /* Save the world, only authorized areas. */
    /* -------------------------------------- */

    if ( !str_cmp( "world", arg1 ) )
    {
	save_area_list();
	for( pArea = area_first; pArea; pArea = pArea->next )
	{
	    /* Builder must be assigned this area. */
	    if ( !IS_BUILDER( ch, pArea ) )
		continue;	  

	    if ( !str_cmp( "verbose", argument ) )
		SET_BIT( pArea->area_flags, AREA_VERBOSE );
	    save_area( pArea );
	    REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED | AREA_VERBOSE );
	}
	send_to_char( "You saved the world.\n\r", ch );
	send_to_all_char( "Database saved.\n\r" );
	return;
    }

    /* Save changed areas, only authorized areas. */
    /* ------------------------------------------ */

    if ( !str_cmp( "changed", arg1 ) )
    {
	char buf[MAX_INPUT_LENGTH];

	save_area_list();

	send_to_char( "Saved zones:\n\r", ch );
	sprintf( buf, "None.\n\r" );

	for( pArea = area_first; pArea; pArea = pArea->next )
	{
	    /* Builder must be assigned this area. */
	    if ( !IS_BUILDER( ch, pArea ) )
		continue;

	    /* Save changed areas. */
	    if ( IS_SET(pArea->area_flags, AREA_CHANGED)
	      || IS_SET(pArea->area_flags, AREA_ADDED) )
	    {
		if ( !str_cmp( "verbose", argument ) )
		    SET_BIT( pArea->area_flags, AREA_VERBOSE );
		save_area( pArea );
		REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED | AREA_VERBOSE );
		sprintf( buf, "%24s - '%s'\n\r", pArea->name, pArea->filename );
		send_to_char( buf, ch );
	    }
        }
	if ( !str_cmp( buf, "None.\n\r" ) )
	    send_to_char( buf, ch );
        return;
    }

    /* Save the area.lst file. */
    /* ----------------------- */
    if ( !str_cmp( arg1, "list" ) )
    {
	save_area_list();
	return;
    }

    /* Save area being edited, if authorized. */
    /* -------------------------------------- */
    if ( !str_cmp( arg1, "area" ) )
    {	
	/* Find the area to save. */
	switch (ch->desc->editor)
	{
	    case ED_AREA:
		pArea = (AREA_DATA *)ch->desc->pEdit;
		break;
	    case ED_ROOM:
		pArea = ch->in_room->area;
		break;
	    case ED_OBJECT:
		pArea = ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area;
		break;
	    case ED_MOBILE:
		pArea = ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area;
		break;
	    default:
		pArea = ch->in_room->area;
		break;
	}

	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "You are not a builder for this area.\n\r", ch );
	    return;
	}

	save_area_list();
	if ( !str_cmp( "verbose", argument ) )
	    SET_BIT( pArea->area_flags, AREA_VERBOSE );
	save_area( pArea );
	REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED | AREA_VERBOSE );
	send_to_char( "Area saved.\n\r", ch );
	return;
    }

    /* Show correct syntax. */
    /* -------------------- */
    do_asave( ch, "" );
    return;
}
