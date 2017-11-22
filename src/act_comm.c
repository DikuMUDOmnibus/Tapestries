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



/*
 * Local functions.
 */
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch, int spool ) );
void	note_remove	args( ( CHAR_DATA *ch, NOTE_DATA *pnote, int spool ) );

void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );


bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if( is_name( "all", pnote->to_list ) )
	return TRUE;

    if( IS_HERO( ch )
     && is_name( "immortal", pnote->to_list ) )
	return TRUE;

    if( is_name_old( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}



void note_attach( CHAR_DATA *ch, int spool )
{
    NOTE_DATA	*pnote;

    if( ch->pnote )
	return;

    if( !note_free )
    {
	pnote	  = alloc_perm( sizeof( *ch->pnote ) );
    }
    else
    {
	pnote	  = note_free;
	note_free = note_free->next;
    }

    pnote->next		= NULL;
    pnote->spool	= spool;
    pnote->anon		= FALSE;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    ch->pnote		= pnote;
    return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, int spool )
{
    NOTE_DATA	*prev;
    FILE	*fp;
    char	*to_list;
    char	to_new[ MAX_INPUT_LENGTH ];
    char	to_one[ MAX_INPUT_LENGTH ];
    char	buf[   MAX_STRING_LENGTH ];

    switch( spool )
    {
	case( SPOOL_NOTE ):
	    sprintf( buf, "note" );
	    break;
	case( SPOOL_IDEA ):
	    sprintf( buf, "idea" );
	    break;
	case( SPOOL_MAIL ):
	    sprintf( buf, "mail" );
	    break;
	case( SPOOL_BUG ):
	    sprintf( buf, "bug" );
	    break;
	case( SPOOL_QUEST ):
	    sprintf( buf, "qwest" );
	    break;
	default:
	    sprintf( buf, "note" );
	    break;
    }

    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[0]	= '\0';
    to_list	= pnote->to_list;
    while( *to_list != '\0' )
    {
	to_list	= one_argument( to_list, to_one );
	if( to_one[ 0 ] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " " );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
    if( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if( pnote == note_list )
    {
	note_list = pnote->next;
    }
    else
    {
	for( prev = note_list ; prev ; prev = prev->next )
	{
	    if( prev->next == pnote )
		break;
	}

	if( !prev )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->spool	= SPOOL_NOTE;
    pnote->anon 	= FALSE;
    pnote->next		= note_free;
    note_free		= pnote;

    /*
     * Rewrite entire list.
     */
    fclose( fpReserve );
    if( !( fp = fopen( NOTE_FILE, "w" ) ) )
    {
	perror( NOTE_FILE );
    }
    else
    {
	for( pnote = note_list ; pnote ; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender );
	    fprintf( fp, "Spool   %d\n",  pnote->spool      );
	    fprintf( fp, "Anon    %d\n",  pnote->anon       );
	    fprintf( fp, "Date    %s~\n", pnote->date );
	    fprintf( fp, "Stamp   %ld\n", pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list );
	    fprintf( fp, "Subject %s~\n", pnote->subject );
	    fprintf( fp, "Text\n%s~\n\n", pnote->text );
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void do_note( CHAR_DATA *ch, char *argument )
{
    note_process( ch, argument, SPOOL_NOTE );
}
void do_idea( CHAR_DATA *ch, char *argument )
{
    note_process( ch, argument, SPOOL_IDEA );
}
void do_mail( CHAR_DATA *ch, char *argument )
{
    note_process( ch, argument, SPOOL_MAIL );
}
void do_bug( CHAR_DATA *ch, char *argument )
{
    note_process( ch, argument, SPOOL_BUG );
}
void do_qwest( CHAR_DATA *ch, char *argument )
{
    note_process( ch, argument, SPOOL_QUEST );
}

void note_process( CHAR_DATA *ch, char *argument, int spool )
{
    NOTE_DATA	*pnote;
    char	*buf1;
    char	buf[  MAX_STRING_LENGTH     ];
    char	buf2[ MAX_STRING_LENGTH     ];
    char	spoolbuf[ MAX_STRING_LENGTH ];
    char	arg[  MAX_INPUT_LENGTH      ];
    int		vnum;
    int		anum;

    if( IS_NPC( ch ) )
	return;

    buf1	= NULL;

    argument	= one_argument( argument, arg );
    smash_tilde( argument );

    switch( spool )
    {
	case( SPOOL_NOTE ):
	    sprintf( buf2, "note" );
	    break;
	case( SPOOL_IDEA ):
	    sprintf( buf2, "idea" );
	    break;
	case( SPOOL_MAIL ):
	    sprintf( buf2, "mail" );
	    break;
	case( SPOOL_BUG ):
	    sprintf( buf2, "bug" );
	    break;
	case( SPOOL_QUEST ):
	    sprintf( buf2, "qwest" );
	    break;
	default:
	    sprintf( buf2, "note" );
	    break;
    }

    if( !*arg )
    {
	switch( spool )
	{
	    case SPOOL_NOTE:
		do_note( ch, "read" );
		break;
	    case SPOOL_IDEA:
		do_idea( ch, "read" );
		break;
	    case SPOOL_BUG:
		do_bug( ch, "read" );
		break;
	    case SPOOL_QUEST:
		do_qwest( ch, "read" );
		break;
	    case SPOOL_MAIL:
		do_mail( ch, "read" );
		break;
	    default:
		return;
	}
	return;
    }

    if( ( !str_prefix( arg, "list" ) ) )
    {
	vnum    = 0;
	buf1	= NULL;
	for( pnote = note_list ; pnote ; pnote = pnote->next )
	{
	    if( ( is_note_to( ch, pnote ) ) && ( spool == pnote->spool ) )
	    {
		if( pnote->anon )
		{
		    sprintf( buf, "[{g%3d{r%s{x] {cAnonymous{x: %s\n\r",
			vnum,
			( pnote->date_stamp > ch->last_note[spool]
			 && str_cmp( pnote->sender, ch->name ) ) ? "N" : " ",
			pnote->subject );
		}
		else
		{
		    sprintf( buf, "[{g%3d{r%s{x] {c%s{x: %s\n\r",
			vnum,
			( pnote->date_stamp > ch->last_note[ spool ]
			 && str_cmp( pnote->sender, ch->name ) ) ? "N" : " ",
			pnote->sender, pnote->subject );
		}
		str_cat( &buf1, buf );
		vnum++;
	    }
	}
	page_to_char( buf1, ch );
	free( buf1 );

	return;
    }

    if( !str_prefix( arg, "read" ) )
    {
	bool fAll;

	if( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if( !*argument || !str_prefix( argument, "next" ) )
	  /* read next unread note */
	{
	    vnum	= 0;
	    buf1	= NULL;
	    for( pnote = note_list ; pnote ; pnote = pnote->next )
	    {
		if( is_note_to( ch, pnote )
		 && str_cmp( ch->name, pnote->sender )
		 && ch->last_note[spool] < pnote->date_stamp
		 && pnote->spool == spool )
		{
		    if( pnote->anon )
		    {
			sprintf( buf, "[{g%3d{x] {cAnonymous{x: %s\n\r%s\n\rTo: %s\n\r",
			    vnum,
			    pnote->subject,
			    pnote->date,
			    pnote->to_list );
		    }
		    else
		    {
			sprintf( buf, "[{g%3d{x] {c%s{x: %s\n\r%s\n\rTo: %s\n\r",
			    vnum,
			    pnote->sender,
			    pnote->subject,
			    pnote->date,
			    pnote->to_list );
		    }
		    str_cat( &buf1, buf );
		    str_cat( &buf1, pnote->text );
		    ch->last_note[spool] = UMAX( ch->last_note[spool],
						 pnote->date_stamp );
		    page_to_char( buf1, ch );
		    free( buf1 );
		    return;
		}
		else if( pnote->spool == spool )
		    vnum++;
	    }
	    sprintf( spoolbuf, "You have no unread %ss.\n\r", buf2 );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}
	else if( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    sprintf( spoolbuf, "%s read which number\n\r", buf2 );
	    spoolbuf[0] = UPPER( spoolbuf[0] );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}

	vnum    = 0;
	buf1	= NULL;
	for( pnote = note_list ; pnote ; pnote = pnote->next )
	{
	    if( is_note_to( ch, pnote ) 
	     && ( pnote->spool == spool )
	     && ( vnum++ == anum || fAll ) )
	    {
		if( pnote->anon )
		{
		    sprintf( buf, "[{g%3d{x] {cAnonymous{x: %s\n\r%s\n\rTo: %s\n\r",
			vnum - 1,
			pnote->subject,
			pnote->date,
			pnote->to_list );
		}
		else
		{
		    sprintf( buf, "[{g%3d{x] {c%s{x: %s\n\r%s\n\rTo: %s\n\r",
			vnum - 1,
			pnote->sender,
			pnote->subject,
			pnote->date,
			pnote->to_list );
		}
		str_cat( &buf1, buf );
		str_cat( &buf1, pnote->text );
		if ( !fAll )
		    page_to_char( buf1, ch );
		else
		    str_cat( &buf1, "\n\r" );
		ch->last_note[spool] = UMAX( ch->last_note[spool], pnote->date_stamp );
		if ( !fAll )
		{
		    free( buf1 );
		    return;
		}
	    }
	}

	sprintf( spoolbuf, "No such %s.\n\r", buf2 );
	if( !fAll )
	    send_to_char_bw( spoolbuf, ch );
	else
	    page_to_char( buf1, ch );

	free( buf1 );
	return;
    }

    if( !str_cmp( arg, "+" ) )
    {
	note_attach( ch, spool );
	strcpy( buf, ch->pnote->text );
	if( strlen( buf ) + strlen( argument ) >= MAX_STRING_LENGTH - 200 )
	{
	    sprintf( spoolbuf, "%s too long.\n\r", buf2 );
	    spoolbuf[0] = UPPER( spoolbuf[0] );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r"   );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char_bw( "Ok, line added.\n\r", ch );
	return;
    }

/*
 * 'note help' function add by Ant, Nov '94
 */
    if( !str_prefix( arg, "help" ) )
    {
	do_help( ch, buf2 );
	return;
    }
/*
 * 'note -' function add by Ant, Nov '94
 * Updated -Ant, Jan '96
 */
    if( !str_cmp( arg, "-" ) )
    {
	if( !ch->pnote )
	{
	    sprintf( spoolbuf, 
		"Hmmm....better make a %s before chopping lines from it.\n\r", 
		buf2 );
	    send_to_char_bw( spoolbuf, ch );
	}
	else
	{
	    strcpy( buf, ch->pnote->text );
	    free_string( ch->pnote->text );
	    ch->pnote->text = strip_last_line( buf );
	    send_to_char_bw( "Line Removed.\n\r", ch );
	}
	return;
    }

    if( !str_cmp( arg, "anonymous" ) )
    {
	note_attach( ch, spool );
	send_to_char_bw( "Anonymous toggled: ", ch );
	if( ch->pnote->anon )
	{
	    ch->pnote->anon = FALSE;
	    send_to_char_bw( "Off.\n\r", ch );
	}
	else
	{
	    ch->pnote->anon = TRUE;
	    send_to_char_bw( "On.\n\r", ch );
	}
	return;
    }

    if( !str_prefix( arg, "subject" ) )
    {
	note_attach( ch, spool );
	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	send_to_char_bw( "Ok.\n\r", ch );
	return;
    }

    if( !str_prefix( arg, "from" ) && IS_IMMORTAL( ch ) )
    {
	note_attach( ch, spool );
	free_string( ch->pnote->sender );
	ch->pnote->sender = str_dup( argument );
	send_to_char_bw( "Ok.\n\r", ch );
	return;
    }

    if( !str_prefix( arg, "to" ) )
    {
	note_attach( ch, spool );
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	send_to_char_bw( "Ok.\n\r", ch );
	return;
    }

    if( !str_prefix( arg, "clear" ) )
    {
	if ( ch->pnote )
	{
	    free_string( ch->pnote->text    );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date    );
	    free_string( ch->pnote->sender  );
	    ch->pnote->anon 	= FALSE;
	    ch->pnote->spool	= SPOOL_NOTE;
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}

	send_to_char_bw( "Ok.\n\r", ch );
	return;
    }

    if( !str_prefix( arg, "show" ) )
    {
	if ( !ch->pnote )
	{
	    sprintf( spoolbuf, "You have no %s in progress.\n\r", buf2 );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}

	switch( ch->pnote->spool )
	{
	case( SPOOL_NOTE ):
	    sprintf( buf2, "note" );
	    break;
	case( SPOOL_IDEA ):
	    sprintf( buf2, "idea" );
	    break;
	case( SPOOL_MAIL ):
	    sprintf( buf2, "mail" );
	    break;
	case( SPOOL_BUG ):
	    sprintf( buf2, "bug" );
	    break;
	case( SPOOL_QUEST ):
	    sprintf( buf2, "qwest" );
	    break;
	default:
	    sprintf( buf2, "note" );
	    break;
	}
	sprintf( buf, "{c%s{x: %s\n\rTo: %s\n\rSpool: %s\n\r",
		ch->pnote->anon ? "Anonymous" : ch->pnote->sender,
		ch->pnote->subject,
		ch->pnote->to_list,
		buf2 );
	str_cat( &buf1, buf );
	str_cat( &buf1, ch->pnote->text );

	page_to_char( buf1, ch );
	free( buf1 );

	return;
    }

    if( !str_prefix( arg, "post" ) || !str_prefix( arg, "send" ) )
    {
	FILE *fp;
	char *strtime;

	if( ( spool == SPOOL_QUEST ) && !IS_IMMORTAL( ch ) )
	{
	    send_to_char_bw( 
		"Only Immortals can post to this spool, note deleted.\n\r", 
		ch );
	    do_qwest( ch, "clear" );
	    return;
	}

	if( !ch->pnote )
	{
	    sprintf( spoolbuf, "You have no %s in progress.\n\r", buf2 );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}

	if( !str_cmp( ch->pnote->to_list, "" ) )
	{
	    send_to_char_bw(
	      "You need to provide a recipient (name, all, imm, gods, or imp).\n\r",
			 ch );
	    return;
	}

	if( !str_cmp( ch->pnote->subject, "" ) )
	{
	    send_to_char_bw( "You need to provide a subject.\n\r", ch );
	    return;
	}

	ch->pnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	free_string( ch->pnote->date );
	ch->pnote->date			= str_dup( strtime );
	ch->pnote->date_stamp           = current_time;

	if( !note_list )
	{
	    note_list	= ch->pnote;
	}
	else
	{
	    for( pnote = note_list; pnote->next; pnote = pnote->next )
		;
	    pnote->next	= ch->pnote;
	}
	pnote		= ch->pnote;
	ch->pnote       = NULL;

	fclose( fpReserve );
	if( !( fp = fopen( NOTE_FILE, "a" ) ) )
	{
	    perror( NOTE_FILE );
	}
	else
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    fprintf( fp, "Spool   %d\n",  pnote->spool      );
	    fprintf( fp, "Anon    %d\n",  pnote->anon       );
	    fprintf( fp, "Date    %s~\n", pnote->date       );
	    fprintf( fp, "Stamp   %ld\n", pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list    );
	    fprintf( fp, "Subject %s~\n", pnote->subject    );
	    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );

	sprintf( spoolbuf, "Your %s has been posted.\n\r", buf2 );
	send_to_char_bw( spoolbuf, ch );
	return;
    }

/*
 * Fixed and updated, Jan '96 -Ant
 */
    if( !str_prefix( arg, "remove" ) )
    {
	if( !is_number( argument ) )
	{
	    sprintf( spoolbuf, "%s remove which number?\n\r", buf2 );
	    spoolbuf[0] = UPPER( spoolbuf[0] );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for( pnote = note_list ; pnote ; pnote = pnote->next )
	{
	    if( is_note_to( ch, pnote ) 
	     && ( pnote->spool == spool )
	     && ( vnum++ == anum ) )
	    {
		note_remove( ch, pnote, spool );
		send_to_char_bw( "Ok.\n\r", ch );
		return;
	    }
	}

	sprintf( spoolbuf, "No such %s.\n\r", buf2 );
	send_to_char_bw( spoolbuf, ch );
	return;
    }

/*
 * Updated, Jan '96 -Ant
 */
    if( !str_prefix( arg, "delete" ) && get_trust( ch ) >= 59 )
    {
	if( !is_number( argument ) )
	{
	    sprintf( spoolbuf, "%s delete which number?\n\r", buf2 );
	    spoolbuf[0] = UPPER( spoolbuf[0] );
	    send_to_char_bw( spoolbuf, ch );
	    return;
	}
 
        anum = atoi( argument );
        vnum = 0;
        for( pnote = note_list ; pnote ; pnote = pnote->next )
        {
            if( spool == pnote->spool 
	      && is_note_to( ch, pnote )
	      && vnum++ == anum )
            {
                note_delete( pnote );
                send_to_char_bw( "Ok.\n\r", ch );
                return;
            }
        }
 
	sprintf( spoolbuf, "No such %s\n\r", buf2 );
	send_to_char_bw( spoolbuf, ch );
        return;
    }

    sprintf( spoolbuf, "Huh?  Type 'help %s' for usage.\n\r", buf2 );
    send_to_char_bw( spoolbuf, ch );

    if( buf1 )
	free( buf1 );

    return;
}

void note_delete( NOTE_DATA *pnote )
{
    FILE 	*fp;
    NOTE_DATA 	*prev;
 
    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
    {
        note_list = pnote->next;
    }
    else
    {
        for ( prev = note_list ; prev ; prev = prev->next )
        {
            if ( prev->next == pnote )
                break;
        }
 
        if ( prev == NULL )
        {
            bug( "Delete: pnote not found.", 0 );
            return;
        }
 
        prev->next = pnote->next;
    }
 
    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->spool	= SPOOL_NOTE;
    pnote->anon 	= FALSE;
    pnote->next		= note_free;
    note_free		= pnote;

 
    /*
     * Rewrite entire list.
     */
    fclose( fpReserve );
    if ( !( fp = fopen( NOTE_FILE, "w" ) ) )
    {
        perror( NOTE_FILE );
    }
    else
    {
        for ( pnote = note_list ; pnote ; pnote = pnote->next )
        {
            fprintf( fp, "Sender  %s~\n", pnote->sender		);
	    fprintf( fp, "Spool   %d\n",  pnote->spool      	);
	    fprintf( fp, "Anon    %d\n",  pnote->anon       	);
            fprintf( fp, "Date    %s~\n", pnote->date		);
            fprintf( fp, "Stamp   %ld\n",  pnote->date_stamp	);
            fprintf( fp, "To      %s~\n", pnote->to_list	);
            fprintf( fp, "Subject %s~\n", pnote->subject	);
            fprintf( fp, "Text\n%s~\n",   pnote->text		);
        }
        fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_chat( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA	*d;
    char		buf[ MAX_STRING_LENGTH ];
    bool		switched = FALSE;
 
    if( argument[0] == '\0' )
    {
	if( IS_SET( ch->chan, CHAN_CHAT ) )
	{
	    send_to_char_bw( "Chat channel is now OFF.\n\r", ch );
	    REMOVE_BIT( ch->chan, CHAN_CHAT );
	}
	else
	{
	    send_to_char_bw( "Chat channel is now ON.\n\r", ch );
	    SET_BIT( ch->chan, CHAN_CHAT );
	}
    }
    else
    {
	if( IS_SET( ch->pen, PEN_NOCHAT ) || IS_SET( ch->pen, PEN_SILENCE ) )
	{
	    send_to_char_bw( "You have been gagged by the administration!\n\r", ch );
	    return;
	}

	SET_BIT( ch->chan, CHAN_CHAT );
 
	if( ch->desc
	 && ch->desc->original
	 && IS_SET( ch->desc->original->act, PLR_SWITCH ) )
	{
	    switched = TRUE;
	}

	sprintf( buf, "{dYou chat '%s'{x\n\r", argument );
	send_to_char( buf, ch );
	for( d = descriptor_list ; d ; d = d->next )
	{
	    CHAR_DATA *victim;
 
	    victim = d->original ? d->original : d->character;
 
	    if( d->connected == CON_PLAYING
	     && d->character != ch
	     && IS_SET( victim->chan, CHAN_CHAT )
	     && !IS_SET( victim->chan, CHAN_QUIET ) )
	    {
		if( switched )
		{
		    act_new( "{y({r$n, switched{y){x",
			ch->desc->original, NULL,
			victim, TO_VICT, POS_SLEEPING );
		}
		act_new( "{d$n chats '$t'{x", 
		   ch, argument, victim, TO_VICT, POS_SLEEPING );
	    }
	}
    }
}



void do_say( CHAR_DATA *ch, char *argument )
{
    if( argument[0] == '\0' )
    {
	send_to_char_bw( "Say what?\n\r", ch );
	return;
    }

    act_new( "{u$n says '$T'.{x",
	ch, NULL, argument, TO_ROOM, POS_RESTING );
    act_new( "{uYou say '$T'.{x",
	ch, NULL, argument, TO_CHAR, POS_RESTING );

    mprog_speech_trigger( argument, ch );
    return;
}

void do_shout( CHAR_DATA *ch, char *argument )
{
    /* TODO */
    return;
}

void do_immtalk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA	*d;
    char		buf[ MAX_STRING_LENGTH ];
    bool		switched = FALSE;
 
    if( argument[0] == '\0' )
    {
	if( IS_SET( ch->chan, CHAN_IMM ) )
	{
	    send_to_char_bw( "Immortal channel is now OFF.\n\r", ch );
	    REMOVE_BIT( ch->chan, CHAN_IMM );
	}
	else
	{
	    send_to_char_bw( "Immortal channel is now ON.\n\r", ch );
	    SET_BIT( ch->chan, CHAN_IMM );
	}
    }
    else
    {
	if( IS_SET( ch->pen, PEN_SILENCE ) )
	{
	    send_to_char_bw( "You have been gagged by the administration!\n\r", ch );
	    return;
	}

	SET_BIT( ch->chan, CHAN_IMM );
 
	if( ch->desc
	 && ch->desc->original
	 && IS_SET( ch->desc->original->act, PLR_SWITCH ) )
	{
	    switched = TRUE;
	}

	sprintf( buf, "$n: %s", argument );
	act_new( "{i$n [{IImmortal{i]: $t{x",
	    ch, argument, NULL, TO_CHAR, POS_DEAD );
	for( d = descriptor_list ; d ; d = d->next )
	{
	    CHAR_DATA *victim;
 
	    victim = d->original ? d->original : d->character;
 
	    if( d->connected <= CON_PLAYING
	     && d->character != ch
	     && IS_SET( victim->chan, CHAN_IMM )
	     && victim->wizbit )
	    {
		if( switched )
		{
		    act_new( "{y({r$n, switched{y){x",
			ch->desc->original, NULL,
			victim, TO_VICT, POS_DEAD );
		}
		act_new("{i$n [{IImmortal{i]: $t{x",
		    ch, argument, victim, TO_VICT, POS_DEAD );
	    }
	}
    }
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;
    char	arg[ MAX_INPUT_LENGTH ];

    if( !IS_NPC( ch ) && IS_SET( ch->pen, PEN_SILENCE ) )
    {
	send_to_char_bw( "Your message didn't get through.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char_bw( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if( !( victim = get_char_world( ch, arg ) )
     || ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char_bw( "They aren't here.\n\r", ch );
	return;
    }

    act_new( "{kYou tell $N '$t'.{x",
	ch, argument, victim, TO_CHAR, POS_SLEEPING );
    act_new( "{k$n tells you '$t'.{x",
	ch, argument, victim, TO_VICT, POS_SLEEPING );
    victim->reply	= ch;

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;

    if( !IS_NPC( ch ) && IS_SET( ch->pen, PEN_SILENCE ) )
    {
	send_to_char_bw( "Your message didn't get through.\n\r", ch );
	return;
    }

    if( !( victim = ch->reply ) )
    {
	send_to_char_bw( "They aren't here.\n\r", ch );
	return;
    }

    if( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act_new( "$E can't hear you.",
	    ch, 0, victim, TO_CHAR, POS_SLEEPING );
	return;
    }

    act_new( "{lYou tell $N '$t'.{x",
	ch, argument, victim, TO_CHAR, POS_SLEEPING );
    act_new( "{l$n tells you '$t'.{x",
	ch, argument, victim, TO_VICT, POS_SLEEPING );
    victim->reply	= ch;

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char	*plast;
    char	buf[ MAX_STRING_LENGTH ];

    if( !IS_NPC( ch ) && IS_SET( ch->act, PEN_NOEMOTE ) )
    {
	send_to_char_bw( "You can't show your emotions.\n\r", ch );
	return;
    }

    if( argument[0] == '\0' )
    {
	send_to_char_bw( "Emote what?\n\r", ch );
	return;
    }

    for( plast = argument ; *plast != '\0' ; plast++ )
	;

    strcpy( buf, argument );
    if( isalpha( plast[-1] ) )
	strcat( buf, "." );

    act_new( "$n $T", ch, NULL, buf, TO_ROOM, POS_RESTING );
    act_new( "$n $T", ch, NULL, buf, TO_CHAR, POS_RESTING );
    return;
}


void do_pose( CHAR_DATA *ch, char *argument )
{
    send_to_char_bw( "Pose removed for re-coding.\n\r", ch );
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char_bw( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA	*d;

    if( IS_NPC( ch ) )
	return;

    if( ch->position == POS_FIGHTING )
    {
	send_to_char_bw( "No way! You are fighting.\n\r", ch );
	return;
    }

    if( ch->position  < POS_STUNNED  )
    {
	send_to_char_bw( "You're not DEAD yet.\n\r", ch );
	return;
    }

    send_to_char( 
	"Where I traveled I came forth, from the earth, into the weave, and out to space again.\n\r",
	ch );
    act_new( "$n has left the game.",
	ch, NULL, NULL, TO_ROOM, POS_RESTING );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    d = ch->desc;
    extract_char( ch, TRUE );
    if( d )
	close_socket( d );

    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if( IS_NPC( ch ) )
	return;

    save_char_obj( ch );
    send_to_char_bw( "Ok.\n\r", ch );

    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;
    char	arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if( arg[ 0 ] == '\0' )
    {
	send_to_char_bw( "Follow whom?\n\r", ch );
	return;
    }

    if( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char_bw( "They aren't here.\n\r", ch );
	return;
    }

    if( IS_AFFECTED( ch, AFF_CHARM )
     && ch->master )
    {
	act( "But you'd rather follow $N!",
	    ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if( victim == ch )
    {
	if( !ch->master )
	{
	    send_to_char_bw( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }
    
    if( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
  
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{

    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) )
	act( "$n stops following you.",
	    ch, NULL, ch->master, TO_VICT );
    act( "You stop following $N.",
	ch, NULL, ch->master, TO_CHAR );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument, FALSE );
	}
    }

    if ( found )
	send_to_char( "Ok.\n\r", ch );
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;
    char	buf[ MAX_STRING_LENGTH ];
    char	arg[ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if( arg[ 0 ] == '\0' )
    {
	CHAR_DATA	*gch;
	CHAR_DATA	*leader;

	leader = ( ch->leader ) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS( leader, ch ) );
	send_to_char( buf, ch );

	for( gch = char_list ; gch ; gch = gch->next )
	{
	    if( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
		    IS_NPC( gch ) ? "Mob " : gch->class->who_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if( ch->master
     || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if( victim->master != ch
     && ch != victim )
    {
	act( "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if( is_same_group( victim, ch )
     && ch != victim )
    {
	victim->leader = NULL;
	act( "$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT );
	act( "$n removes you from $s group.", ch, NULL, victim, TO_VICT   );
	act( "You remove $N from your group.", ch, NULL, victim, TO_CHAR  );
	return;
    }

    victim->leader = ch;
    act( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    act( "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }
    
    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\n\r", ch );
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }
	    
    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    sprintf( buf,
	"You split %d gold coins.  Your share is %d gold coins.\n\r",
	amount, share + extra );
    send_to_char( buf, ch );

    sprintf( buf, "$n splits %d gold coins.  Your share is %d gold coins.",
	amount, share );

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->pen, PEN_NOTELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	    send_to_char( buf, gch );
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Ant Oct 94
 *        revised to include config colour, Ant Feb 95
 */
void do_colour( CHAR_DATA *ch, char *argument )
{
    char 	arg[ MAX_STRING_LENGTH ];

    if( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );

    if( !*arg )
    {
	if( !IS_SET( ch->act, PLR_COLOUR ) )
	{
	    send_to_char_bw( "Colour is now ON, Way Cool!\n\r", ch );
	    SET_BIT( ch->act, PLR_COLOUR );
	}
	else
	{
	    send_to_char_bw( "Colour is now OFF, <sigh>\n\r", ch );
	    REMOVE_BIT( ch->act, PLR_COLOUR );
	}
	return;
    }

    if( !str_cmp( arg, "default" ) )
    {
	default_colour( ch );
	send_to_char_bw( "Colour setting set to default values.\n\r", ch );
	return;
    }

    if( !str_cmp( arg, "all" ) )
    {
	all_colour( ch, argument );
	return;
    }

    if( !str_cmp( arg, "text" ) )
    {
	ALTER_COLOUR( text )
    }
    else if( !str_cmp( arg, "auction" ) )
    {
	ALTER_COLOUR( auction )
    }
    else if( !str_cmp( arg, "chat" ) )
    {
	ALTER_COLOUR( chat )
    }
    else if( !str_cmp( arg, "music" ) )
    {
	ALTER_COLOUR( music )
    }
    else if( !str_cmp( arg, "question" ) )
    {
	ALTER_COLOUR( question )
    }
    else if( !str_cmp( arg, "answer" ) )
    {
	ALTER_COLOUR( answer )
    }
    else if( !str_cmp( arg, "class_text" ) )
    {
	ALTER_COLOUR( class_text )
    }
    else if( !str_cmp( arg, "class_name" ) )
    {
	ALTER_COLOUR( class_name )
    }
    else if( !str_cmp( arg, "immtalk_text" ) )
    {
	ALTER_COLOUR( immtalk_text )
    }
    else if( !str_cmp( arg, "immtalk_type" ) )
    {
	ALTER_COLOUR( immtalk_type )
    }
    else if( !str_cmp( arg, "info" ) )
    {
	ALTER_COLOUR( info )
    }
    else if( !str_cmp( arg, "tell" ) )
    {
	ALTER_COLOUR( tell )
    }
    else if( !str_cmp( arg, "reply" ) )
    {
	ALTER_COLOUR( reply )
    }
    else if( !str_cmp( arg, "say" ) )
    {
	ALTER_COLOUR( say )
    }
    else if( !str_cmp( arg, "gtell_text" ) )
    {
	ALTER_COLOUR( gtell_text )
    }
    else if( !str_cmp( arg, "gtell_type" ) )
    {
	ALTER_COLOUR( gtell_type )
    }
    else if( !str_cmp( arg, "bbro" ) )
    {
	ALTER_COLOUR( bbro )
    }
    else if( !str_cmp( arg, "room_title" ) )
    {
	ALTER_COLOUR( room_title )
    }
    else if( !str_cmp( arg, "room_text" ) )
    {
	ALTER_COLOUR( room_text )
    }
    else if( !str_cmp( arg, "room_exits" ) )
    {
	ALTER_COLOUR( room_exits )
    }
    else if( !str_cmp( arg, "room_things" ) )
    {
	ALTER_COLOUR( room_things )
    }
    else if( !str_cmp( arg, "prompt" ) )
    {
	ALTER_COLOUR( prompt )
    }
    else if( !str_cmp( arg, "fight_death" ) )
    {
	ALTER_COLOUR( fight_death )
    }
    else if( !str_cmp( arg, "fight_yhit" ) )
    {
	ALTER_COLOUR( fight_yhit )
    }
    else if( !str_cmp( arg, "fight_ohit" ) )
    {
	ALTER_COLOUR( fight_ohit )
    }
    else if( !str_cmp( arg, "fight_thit" ) )
    {
	ALTER_COLOUR( fight_thit )
    }
    else if( !str_cmp( arg, "fight_skill" ) )
    {
	ALTER_COLOUR( fight_skill )
    }
    else
    {
	send_to_char_bw( "Unrecognised Colour Parameter Not Set.\n\r", ch );
	return;
    }

    send_to_char_bw( "New Colour Parameter Set.\n\r", ch );
    return;
}

void do_unread( CHAR_DATA *ch, char *argument )
{
    NOTE_DATA	*pnote;
    char	sbuf[ MAX_STRING_LENGTH ];
    char	buf[ MAX_STRING_LENGTH  ];
    int		notes = 0;
    int		spool = 0;

    for( spool = 0 ; spool < MAX_SPOOL ; spool++ )
    {
	notes = 0;
	switch( spool )
	{
	    case( SPOOL_NOTE ):
		sprintf( sbuf, "note" );
		break;
	    case( SPOOL_IDEA ):
		sprintf( sbuf, "idea" );
		break;
	    case( SPOOL_MAIL ):
		sprintf( sbuf, "mail" );
		break;
	    case( SPOOL_BUG ):
		sprintf( sbuf, "bug" );
		break;
	    case( SPOOL_QUEST ):
		sprintf( sbuf, "qwest" );
		break;
	    default:
		sprintf( sbuf, "note" );
		break;
	}
	for( pnote = note_list ; pnote ; pnote = pnote->next )
	{
	    if( is_note_to( ch, pnote ) 
	     && str_cmp( ch->name, pnote->sender )
	     && pnote->spool == spool
	     && pnote->date_stamp > ch->last_note[ spool ] )
		notes++;
	}
	if( notes == 1 )
	{
	    sprintf( buf, "\n\rYou have one new %s waiting.\n\r", sbuf );
	    send_to_char_bw( buf, ch );
	}
	else if( notes > 1 )
	{
	    sprintf( buf, "\n\rYou have %d new %ss waiting.\n\r",
		notes,
		sbuf );
	    send_to_char_bw( buf, ch );
	}
    }
    return;
}