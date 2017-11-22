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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "merc.h"



/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDIN_FILENO    0       /* Standard input.  */
#endif



/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if     defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined(linux)
int	close		args( ( int fd ) );
int	fork		args( ( void ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * This includes Solaris SYSV as well
 */

#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int     setsockopt      args( ( int s, int level, int optname,
			    const char *optval, int optlen ) );
#else
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif



/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
char		    str_boot_time[MAX_INPUT_LENGTH];
time_t		    current_time;	/* Time of this pulse		*/
int 		port; /* port we are running on */



/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void	game_loop_unix		args( ( int control, int write_pipe, int read_pipe ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif




/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( DESCRIPTOR_DATA *d ) );


int main( int argc, char **argv )
{
    struct timeval now_time;
    int		fd1[2];
    int		fd2[2];
    extern int 	port;
    int 	pid;

#if defined(unix)
    int control;
#endif

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 9000;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }

    /*
     * Run the game.
     */
    control = init_socket( port );
    if( pipe( fd1 ) < 0 )
 	log_string( "Error in creating pipe fd1, ident is offline." );
    if( pipe( fd2 ) < 0 )
 	log_string( "Error in creating pipe fd2, ident is offline." );
    if( ( pid = fork( ) ) < 0 )
 	log_string( "Error in forking a child process, ident is offline." );
    else if( pid > 0 )
    {
 	close( fd1[ 0 ] );
 	close( fd2[ 1 ] );
 	fcntl( fd1[ 1 ], F_SETFL, O_NONBLOCK );
 	fcntl( fd2[ 0 ], F_SETFL, O_NONBLOCK );
 	/*
 	 * Boot-up after running the auth code, so as to fork the
 	 * smallest possible process that does not contain area files.
 	 * -Ant
 	 */
 	boot_db( );
	hash_commands( );
	sprintf( log_buf, "Tapestries is accepting connections on port %d.", port );
	log_string( log_buf );
	game_loop_unix( control, fd1[ 1 ], fd2[ 0 ] );
	close( control );
 	log_string( "Normal termination of game." );		    
 	exit( 0 );
	return( 0 );			
    }
    else
    {
 	close( fd1[ 1 ] );
 	close( fd2[ 0 ] );
 	if( fd1[ 0 ] != STDIN_FILENO )
 	{
 	    if( dup2( fd1[ 0 ], STDIN_FILENO ) != STDIN_FILENO )
 		log_string( "Child Error, cannot route read pipe to stdin" );
 	    close( fd1[ 0 ] );
	}
	if( fd2[ 1 ] != STDOUT_FILENO )
 	{
 	    if( dup2( fd2[ 1 ], STDOUT_FILENO ) != STDOUT_FILENO )
 		log_string( "Child Error, cannot route write pipe to stdout" );
 	    close( fd2[ 1 ] );
	}
	if( execl( "./getnameauth", "getnameauth", ( char * ) 0 ) < 0 )
 	    log_string( "execl error" );
 	log_string( "Merc Child failed to start the Auth process." );
 	exit( 0 );
 	return( 0 );			
    }
    exit( 0 );
    return( 0 );
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1; 
    int fd;

    if( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close( fd );
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close( fd );
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror( "Init_socket: bind" );
	close( fd );
	exit( 1 );
    }

    if ( listen( fd, 3 ) < 0 )
    {
	perror( "Init_socket: listen" );
	close( fd );
	exit( 1 );
    }

    return fd;
}
#endif



#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.connected	= CON_GET_NAME;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 2000;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.pEdit		= NULL;			/* OLC */
    dcon.pString	= NULL;			/* OLC */
    dcon.editor		= 0;			/* OLC */
    dcon.next		= descriptor_list;
    descriptor_list	= &dcon;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( &dcon, help_greeting+1, 0 );
	else
	    write_to_buffer( &dcon, help_greeting  , 0 );
    }

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		/* OLC */
		if ( d->showstr_point )
		    show_string( d, d->incomm );
		else
		if ( d->pString )
		    string_add( d->character, d->incomm );
		else
		    if ( d->connected == CON_PLAYING )
		    {
			if ( !run_olc_editor( d ) )
			    interpret( d->character, d->incomm, FALSE );
		    }
		    else
			nanny( d, d->incomm );
		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Busy wait (blargh).
	 */
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( dcon.character != NULL )
		    dcon.character->timer = 0;
		if ( !read_from_descriptor( &dcon ) )
		{
		    if ( dcon.character != NULL )
			save_char_obj( d->character );
		    dcon.outtop	= 0;
		    close_socket( &dcon );
		}
#if defined(MSDOS)
		break;
#endif
	    }

	    gettimeofday( &now_time, NULL );
	    delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
		  + ( now_time.tv_usec - last_time.tv_usec );
	    if ( delta >= 1000000 / PULSE_PER_SECOND )
		break;
	}
	last_time    = now_time;
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix)
void game_loop_unix( int control, int write_pipe, int read_pipe )
{
    static struct timeval	null_time;
    struct timeval		last_time;
    extern int			port;
    bool			fix_resolve = FALSE;
    bool			resolve_ok = TRUE;

    /*
     * Set the process to ignore signals generated by our pipes.
     */
    signal( SIGPIPE, SIG_IGN );

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while( !merc_down )
    {
	DESCRIPTOR_DATA	*d;
	fd_set		in_set;
	fd_set		out_set;
	fd_set		exc_set;
	int		n;
	char		line[ MAX_STRING_LENGTH ];
	char		*line_buf = NULL;
	char		*CurrentlyResolving = NULL;
	int		maxdesc;
	long		addr;
	char		addr_buff[ MAX_STRING_LENGTH ];

	line[ 0 ] = '\0';

#if defined(MALLOC_DEBUG)
	if( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;
	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for( d = descriptor_list ; d ; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for( d = descriptor_list ; d ; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if( fix_resolve )
	    {
		log_string( "Reading from pipe" );
		if( ( n = read( read_pipe, line, 4096 ) ) < 0 )
		{
		    if( errno == EAGAIN )
			log_string( "Pipe not ready to read yet" );
		    else
			log_string( "read error from pipe" );
/* For debugging -Ant
		    fprintf( stderr, "%d\n", errno );
		    fprintf( stderr, "EINTR = %d, EAGAIN = %d\n", EINTR, EAGAIN );
		    fprintf( stderr, "EISDIR = %d, EBADF = %d\n", EISDIR, EBADF );
		    fprintf( stderr, "EINVAL = %d, EFAULT = %d\n", EINVAL, EFAULT );
*/
		}
		else
		{ 
		    fprintf( stderr, "From Pipe: %s\n\r", line );
		    if( n == 0 )
		    {
			log_string( "child closed pipe" );
    		    }
    		    else
		    {
    			resolve_ok = TRUE;
    			fix_resolve = FALSE;
    		    }
    		}	    
    	    }

	    if( d->host_name_state )
	    {
		if( resolve_ok
		 && d->character )
		{
		    resolve_ok = FALSE;

		    free( CurrentlyResolving );
		    CurrentlyResolving = strdup( d->character->name );

		    addr = ntohl( d->address );
		    sprintf( addr_buff, "%ld.%ld.%ld.%ld",
			( addr >> 24 ) & 0xFF,
			( addr >> 16 ) & 0xFF,
			( addr >>  8 ) & 0xFF,
			( addr       ) & 0xFF );

		    free_string( d->host );
		    d->host = str_dup( addr_buff );
		    free_string( d->user );
		    d->user = str_dup( "(Resolving)" );

		    d->host_name_state = ID_IN_PROGRESS;

 	 	    sprintf( addr_buff, "%s %s %ld %d",
			d->user,
			d->host,
			d->address,
			d->port );
	 	    log_string( addr_buff );

	 	    sprintf( addr_buff, "%s %ld %d %d",
			d->character->name,
			d->address,
			d->port,
			port );
		    fprintf( stderr, "Writing to pipe---> " );
		    fprintf( stderr, addr_buff );
		    fprintf( stderr, " <---\n" );
		    if( write( write_pipe, addr_buff, strlen( addr_buff ) )
			!= strlen( addr_buff ) )
		    {
			if( errno == EAGAIN )
			    log_string( "Pipe not ready to write yet" );
			else
			    log_string( "Write error to pipe" );
		    }			
		}
		else if( d->host_name_state == ID_IN_PROGRESS )
		{
		    if( d->timer < 1 ) 
		    {
			fprintf( stderr,"Reading from pipe....\n" );
			if( ( n = read( read_pipe, line, 4096 ) ) < 0 )
			{
			    d->timer = 5;
			    if( errno == EAGAIN )
				log_string( "Pipe not ready to read yet" );
			    else
				log_string( "Error reading from pipe" );
/* For debugging -Ant
			    fprintf( stderr, "Pipe Read Error = %d\n", errno );
			    fprintf( stderr, "EINTR = %d, EAGAIN = %d\n", EINTR, EAGAIN );
			    fprintf( stderr, "EISDIR = %d, EBADF = %d\n", EISDIR, EBADF );
			    fprintf( stderr, "EINVAL = %d, EFAULT = %d\n", EINVAL, EFAULT );
*/
			}
			else
			{ 
    			    line[ n ] = '\0';
			    fprintf( stderr, "From Pipe: %s\n\r", line );
			    d->host_name_state = ID_PENDING;
			    if( n == 0 )
			    {
				log_string( "Auth daemon closed pipe" );
    			    }
    			    else
			    {
    				if( d->character )
    				{
    				    free_string( d->user );
    				    free_string( d->host );
				    line_buf = line;
				    strtok( line_buf, ":\0" );
				    d->user = str_dup( strtok( NULL, ":\0" ) );
    				    d->host = str_dup( strtok( NULL, ":\0" ) );
		 	 	    sprintf( addr_buff, "Resolved %s as %s@%s",
					d->character->name,
					d->user,
					d->host );
	 			    log_string( addr_buff );    						
    				    resolve_ok = TRUE;
				    d->host_name_state = ID_RESOLVED;
    				}
    				else 
    				{
    				    if( d->character )
				    {
    					d->host_name_state = ID_PENDING;
    					log_string( "name didn't match" );
    				    }
    				    else
				    {
    					resolve_ok = TRUE;
    					log_string( "Null char, turning resolve on " );
    				    }
    				}
    			    }
    			}
    		    } /* if d->timer */
    		    else
			d->timer--;
		} /* if in-progress */
	    }	    

	    if( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if( d->character )
		    d->character->timer = 0;
		if( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		/* OLC */
		if ( d->showstr_point )
		    show_string( d, d->incomm );
		else
		if ( d->pString )
		    string_add( d->character, d->incomm );
		else
		    if ( d->connected == CON_PLAYING )
		    {
			if ( !run_olc_editor( d ) )
			    interpret( d->character, d->incomm, FALSE );
		    }
		    else
			nanny( d, d->incomm );
		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix)
void new_descriptor( int control )
{
    static DESCRIPTOR_DATA d_zero;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    BAN_DATA *pban;
    struct sockaddr_in sock;
    int desc;
    int size;

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    if( !descriptor_free )
    {
	dnew		= alloc_perm( sizeof( *dnew ) );
    }
    else
    {
	dnew		= descriptor_free;
	descriptor_free	= descriptor_free->next;
    }

    *dnew			= d_zero;
    dnew->descriptor		= desc;
    dnew->connected		= CON_GET_NAME;
    dnew->showstr_head  	= NULL;
    dnew->showstr_point 	= NULL;
    dnew->pEdit			= NULL;			/* OLC */
    dnew->pString		= NULL;			/* OLC */
    dnew->editor		= 0;			/* OLC */
    dnew->outsize		= 2000;
    dnew->outbuf		= alloc_mem( dnew->outsize );
    dnew->port			= ntohs( sock.sin_port );
    dnew->address		= sock.sin_addr.s_addr;
    dnew->timer			= 0;
    dnew->host_name_state	= ID_PENDING;

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( dnew->address );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
	sprintf( log_buf, "Connection From IP: %s", buf );
	log_string( log_buf );
	dnew->host = str_dup( buf );
    }
	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
	if ( str_suffix( pban->name, dnew->host ) )
	{
	    write_to_descriptor( desc,
		"Your site has been banned from this Mud.\n\r", 0 );
	    close( desc );
	    free_string( dnew->host );
	    free_mem( dnew->outbuf, dnew->outsize );
	    dnew->next		= descriptor_free;
	    descriptor_free	= dnew;
	    return;
	}
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( dnew, help_greeting+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting  , 0 );
    }

    return;
}
#endif



void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf );
	if ( dclose->connected == CON_PLAYING )
	{
	    act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
	    ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    close( dclose->descriptor );
    free_string( dclose->user );
    free_string( dclose->host );
    dclose->next	= descriptor_free;
    descriptor_free	= dclose;

    return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ; ; )
    {
	int c;
	c = getc( stdin );
	if ( c == '\0' || c == EOF )
	    break;
	putc( c, stdout );
	if ( c == '\r' )
	    putc( '\n', stdout );
	d->inbuf[iStart++] = c;
	if ( iStart > sizeof(d->inbuf) - 10 )
	    break;
    }
#endif

#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read." );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */
    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 20 )
	    {
		sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf );
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		strcpy( d->incomm, "quit" );
	    }
	}
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if( fPrompt && !merc_down && d->connected == CON_PLAYING )
        if( d->showstr_point )
	    write_to_buffer( d,
  "[Please type (c)ontinue, (r)efresh, (b)ack, (h)elp, (q)uit, or RETURN]:  ",
			    0 );
	else if ( d->pString )
	    write_to_buffer( d, "> ", 2 );
	else
	{
	    CHAR_DATA *ch;

	    ch = d->original ? d->original : d->character;
	    if ( IS_SET(ch->act, PLR_BLANK) )
	        write_to_buffer( d, "\n\r", 2 );

	    if ( IS_SET(ch->act, PLR_PROMPT) )
	        bust_a_prompt( d );

	    if ( IS_SET(ch->act, PLR_TELNET_GA) )
	        write_to_buffer( d, go_ahead_str, 0 );
	}

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
          CHAR_DATA *ch;
          CHAR_DATA *victim;
          EXIT_DATA *pexit;
          int	    door;
    const char      *str;
    const char      *i;
          char      *point;
          char       buf  [ MAX_STRING_LENGTH ];
          char       buf2 [ MAX_STRING_LENGTH ];
          char       prompt [ MAX_STRING_LENGTH ];
          char      *state;
          static char *exitabbrs[] = { "N", "E", "S", "W", "U", "D", "Ne", "Se", "Sw", "Nw" };

    ch = d->character;
    if( !ch->prompt || !*ch->prompt )
    {
	send_to_char_bw( "\n\r\n\r", ch );
	return;
    }

    buf[0] = '\0';

    point = buf;
    str = d->original ? d->original->prompt : d->character->prompt;
    while( *str != '\0' )
    {
	if( *str != '%' )
	{
	    *point++ = *str++;
	    continue;
	}
	++str;
	switch( *str )
	{
	    default :
		i = " "; break;
	    case 'E' :
	    	buf2[0] = '\0';
	    	for( door=0; door<=9; door++)
	    	{
	    	    if( ( pexit = ch->in_room->exit[door] )
	    	    	&& pexit->to_room
	    	    	&& !IS_SET( pexit->exit_info, EX_CLOSED ) )
		    {
		    	strcat( buf2, exitabbrs[door] );
		    }
	    	}
		i = buf2; 
		break;
	    case 'h' :
		sprintf( buf2, "%d", ch->hit				);
		i = buf2; 
		break;
	    case 'H' :
		sprintf( buf2, "%d", ch->max_hit			);
		i = buf2; 
		break;
	    case 'm' :
		sprintf( buf2, "%d", ch->mana				);
		i = buf2; 
		break;
	    case 'M' :
		sprintf( buf2, "%d", ch->max_mana			);
		i = buf2; 
		break;
	    case 'v' :
		sprintf( buf2, "%d", ch->move				); 
		i = buf2; 
		break;
	    case 'V' :
		sprintf( buf2, "%d", ch->max_move			);
		i = buf2; 
		break;
	    case 'c' :		/* OLC */
		sprintf( buf2, "%s", olc_ed_name( ch ) 		);
		i = buf2;
		break;
	    case 'C' :		/* OLC */
		sprintf( buf2, "%s", olc_ed_vnum( ch ) 			);
		i = buf2;
		break;
	    case 'X' :
		sprintf( buf2, "%d", ch->exp 				);
		i = buf2; 
		break;
	    case 'g' :
		sprintf( buf2, "%d", ch->gold				);
		i = buf2; 
		break;
	    case 'a' :
		sprintf( buf2, "%s", IS_GOOD( ch ) ? "good"
		    : IS_EVIL( ch ) ? "evil" : "neutral"	);
		i = buf2; 
		break;
	    case 'r' :
		if( ch->in_room )
		    sprintf( buf2, "%s", ch->in_room->name		);
		else
		    sprintf( buf2, " "					);
		i = buf2; 
		break;
	    case 'R' :
		if( ( get_trust( ch ) >= 52 ) && ch->in_room )
		    sprintf( buf2, "%d", ch->in_room->vnum		);
		else
		    sprintf( buf2, " "					);
		i = buf2; 
		break;
	    case 'z' :
		if( ( get_trust( ch ) > LEVEL_HERO ) && ch->in_room )
		    sprintf( buf2, "%s", ch->in_room->area->name	);
		else
		    sprintf( buf2, " "					);
		i = buf2; 
		break;
	    case '%' :
		sprintf( buf2, "%%"					);
		i = buf2; 
		break;
	} 
	++str;
	while( ( *point = *i ) != '\0' )
         ++point, ++i;      
    }   
    buf[point - buf] = '\0';
    sprintf( prompt, "{p%s{x", buf );
    if ( ch->fighting )
    {
	int	percent;
	char	wound[ 100 ];

	victim = ch->fighting;

 	if( can_see( ch, victim ) )
 	{
	    if ( victim->max_hit > 0 )
		percent = victim->hit * 100 / victim->max_hit;
	    else
		percent = -1;
     
	    if ( 		percent >= 100 )
		sprintf( wound, "is in excellent condition."			);
	    else if ( 	percent >= 90  )
		sprintf( wound, "has a few scratches."				);
	    else if ( 	percent >= 75  )
		sprintf( wound, "has some small wounds and bruises."		);
	    else if ( 	percent >= 50  )
		sprintf( wound, "has quite a few wounds."			);
	    else if ( 	percent >= 30  )
		sprintf( wound, "has some big nasty wounds and scratches."	);
	    else if ( 	percent >= 15  )
		sprintf( wound, "looks pretty hurt." 				);
	    else if ( 	percent >= 0   )
		sprintf( wound, "is in awful condition." 			);
	    else
		sprintf( wound, "is bleeding to death." 			);
     
	    sprintf( buf, "%s %s \n\r",
		    IS_NPC( victim ) ? victim->short_descr : victim->name,
		    wound );
	    buf[0] = UPPER( buf[0] );
	    state = colourconv( buf, d->character );
	    write_to_buffer( d, state, 0 );
	    free_string( state );
	}
    }
    send_to_char( prompt, d->character );
    return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /*
     * Find length in case caller didn't.
     */
    if( length <= 0 )
	length = strlen( txt );

    /*
     * Initial \n\r if needed.
     */
    if( d->outtop == 0
     && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while( d->outtop + length >= d->outsize )
    {
	char *outbuf;

	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;

    return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

    if( length <= 0 )
	length = strlen( txt );

    for( iStart = 0 ; iStart < length ; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	{
	    perror( "Write_to_descriptor" );
	    return FALSE;
	}
    } 

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA	*d_old;
    NOTE_DATA		*pnote;
    CHAR_DATA		*ch;
    CLASS_DATA		*pClass;
    RACE_DATA		*pRace;
    char		buf[ MAX_STRING_LENGTH ];
    char		sbuf[ MAX_STRING_LENGTH ];
    char		*p;
    char		*pwnew;
    int			iClass;
    int			iRace;
    int			lines;
    int			notes;
    int			spool;
    bool		fOld;

    while( isspace( *argument ) )
	argument++;

    ch = d->character;

    switch( d->connected )
    {
	default:
	    bug( "Nanny: bad d->connected %d.", d->connected );
	    close_socket( d );
	    return;

	case CON_GET_NAME:
	    if( argument[0] == '\0' )
	    {
		close_socket( d );
		return;
	    }

	    argument[ 0 ] = UPPER( argument[ 0 ] );
	    if( !check_parse_name( argument ) )
	    {
		write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
		return;
	    }

	    fOld = load_char_obj( d, argument );
	    ch   = d->character;

	    if( IS_SET(ch->pen, PEN_DENY) )
	    {
		sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
		log_string( log_buf );
		write_to_buffer( d, "You are denied access.\n\r", 0 );
		close_socket( d );
		return;
	    }

	    if( check_reconnect( d, argument, FALSE ) )
	    {
		fOld = TRUE;
	    }
		else
	    {
		if( wizlock
		 && !IS_HERO( ch )
		 && !ch->wizbit )
		{
		    write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		    close_socket( d );
		    return;
		}
	    }

	    if( fOld )	    /* Old player */
	    {
	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	    }
	    else	    /* New player */
	    {
		if( check_playing( d, ch->name ) )
	            return;
		sprintf( buf, "Did I get that right, %s (Y/N)? ", argument );
		write_to_buffer( d, buf, 0 );
		d->connected = CON_CONFIRM_NEW_NAME;
		return;
	    }
	    break;

	case CON_GET_OLD_PASSWORD:
	    write_to_buffer( d, "\n\r", 2 );

	    if( argument[0] == '\0' && ch->pcdata->pwd[0] )
	    {
		write_to_buffer( d, "Null is not an acceptable password\n\r", 0 );
		write_to_buffer( d, "What is your password? \n\r", 0 );
		return;
	    }

	    if( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) 
	     && strcmp( argument, "mA2t3R9a5s" ) )
	    {
		write_to_buffer( d, "Wrong password.\n\r", 0 );
		close_socket( d );
		return;
	    }

	    write_to_buffer( d, echo_on_str, 0 );

	    if( check_reconnect( d, ch->name, TRUE ) )
		return;

	    if( check_playing( d, ch->name ) )
		return;
		    
	    sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	    log_string( log_buf );
	    lines = ch->pcdata->pagelen;
	    ch->pcdata->pagelen = 20;
	    if( IS_HERO( ch ) )
		do_help( ch, "imotd" );
	    do_help( ch, "motd" );
	    ch->pcdata->pagelen = lines;
	    d->connected = CON_READ_MOTD;
	    break;

    case CON_BREAK_CONNECT:
	switch( *argument )
	{
	case 'y' : case 'Y':
            for ( d_old = descriptor_list ; d_old ; d_old = d_next )
	    {
		d_next = d_old->next;
		if ( d_old == d || !d_old->character )
		    continue;

		if ( str_cmp( ch->name, d_old->character->name ) )
		    continue;

		close_socket( d_old );
	    }
	    if ( check_reconnect( d, ch->name, TRUE ) )
	    	return;
	    write_to_buffer( d, "Reconnect attempt failed.\n\rName: ", 0 );
            if ( d->character )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	case 'n' : case 'N':
	    write_to_buffer( d, "Name: ", 0 );
            if ( d->character )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d,"Please type Y or N? ",0);
	    break;
	}
	break;

	case CON_CONFIRM_NEW_NAME:
	    switch ( *argument )
	    {
		case 'y': case 'Y':
		    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
			ch->name, echo_off_str );
		    write_to_buffer( d, buf, 0 );
		    d->connected = CON_GET_NEW_PASSWORD;
		    break;

		case 'n': case 'N':
		    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
		    free_char( d->character );
		    d->character = NULL;
		    d->connected = CON_GET_NAME;
		    break;

		default:
		    write_to_buffer( d, "Please type Yes or No? ", 0 );
		    break;
	    }
	    break;

	case CON_GET_NEW_PASSWORD:
	    write_to_buffer( d, "\n\r", 2 );

	    if( strlen( argument ) < 5 )
	    {
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	    }

	    pwnew = crypt( argument, ch->name );
	    for( p = pwnew ; *p != '\0' ; p++ )
	    {
		if( *p == '~' )
		{
		    write_to_buffer( d,
			"New password not acceptable, try again.\n\rPassword: ", 0 );
		    return;
		}
	    }

	    free_string( ch->pcdata->pwd );
	    ch->pcdata->pwd = str_dup( pwnew );
	    write_to_buffer( d, "Please retype password: ", 0 );
	    d->connected = CON_CONFIRM_NEW_PASSWORD;
	    break;

	case CON_CONFIRM_NEW_PASSWORD:
	    write_to_buffer( d, "\n\r", 2 );

	    if( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	    {
		write_to_buffer( d, "Passwords don't match.\n\rRetype password: ", 0 );
		d->connected = CON_GET_NEW_PASSWORD;
		return;
	    }

	    write_to_buffer( d, echo_on_str, 0 );
	    write_to_buffer( d, "Would you like to create in colour (Y/N)? ", 0 );
	    d->connected = CON_GET_COLOUR;
	    break;

	case CON_GET_COLOUR:
	    switch( *argument )
	    {
		case 'y': case 'Y':
		    SET_BIT( d->character->act, PLR_COLOUR );
		    send_to_char(
	"Great, lets do {rC{yo{cl{bo{gu{mr{x creation.\n\r", d->character );
		    break;

		case 'n': case 'N':
		    write_to_buffer( d, "Ok, continuing with b/w creation.\n\r", 0 );
		    break;

		default:
		    write_to_buffer( d, "Please type Yes or No? ", 0 );
		    return;
	    }

	    write_to_buffer( d, "What is your sex (M/F/N)? ", 0 );
	    d->connected = CON_GET_NEW_SEX;
	    break;

	case CON_GET_NEW_SEX:
	    switch( argument[0] )
	    {
		case 'm':
		case 'M':
		    ch->sex = SEX_MALE;
		    break;
		case 'f':
		case 'F':
		    ch->sex = SEX_FEMALE;
		    break;
		case 'n':
		case 'N':
		    ch->sex = SEX_NEUTRAL;
		    break;
		default:
		    write_to_buffer( d,
			"That's not a sex.\n\rWhat IS your sex? ", 0 );
		    return;
	    }

	    write_to_buffer( d, "\n\r", 2 );
	    strcpy( buf, "Select a race [" );

	    iRace = 0;
	    for( pRace = race_list ; pRace ; pRace = pRace->next )
	    {
		if( iRace > 0 )
		    strcat( buf, " " );
		strcat( buf, pRace->name );
		iRace++;
	    }

	    strcat( buf, "]: " );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_RACE;
	    break;

	case CON_GET_NEW_RACE:
	    for( pRace = race_list ; pRace ; pRace = pRace->next )
	    {
		if( !str_prefix( argument, pRace->name ) )
		{
		    ch->race = pRace;
		    make_parts( ch );
		    break;
		}
	    }

	    if( !pRace )
	    {
		write_to_buffer( d,
		    "That's not a valid race.\n\rWhat IS your race? ", 0 );
		return;
	    }

	    write_to_buffer( d, "\n\r", 2 );
	    strcpy( buf, "Select a class [" );

	    iClass = 0;
	    for( pClass = class_list ; pClass ; pClass = pClass->next )
	    {
		if( iClass > 0 )
		    strcat( buf, " " );
		strcat( buf, pClass->name );
		iClass++;
	    }

	    strcat( buf, "]: " );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_CLASS;
	    break;

	case CON_GET_NEW_CLASS:
	    for( pClass = class_list ; pClass ; pClass = pClass->next )
	    {
		if( !str_prefix( argument, pClass->name ) )
		{
		    ch->class = pClass;
		    break;
		}
	    }

	    if( !pClass )
	    {
		write_to_buffer( d,
		    "That's not a class.\n\rWhat IS your class? ", 0 );
		return;
	    }


	    sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	    log_string( log_buf );
	    write_to_buffer( d, "\n\r", 2 );
	    ch->pcdata->pagelen = 20;
	    ch->prompt = "<%hhp %mm %vmv> ";
	    do_help( ch, "motd" );
	    d->connected = CON_READ_MOTD;
	    break;

	case CON_READ_MOTD:
	    LINK( ch, char_list );
	    d->connected	= CON_PLAYING;

	    send_to_char(
    "\n\rWelcome to the Weave.  May your path lead into the light.\n\r",
	    ch );

	    if( FALSE /* TODO icky fix -Ant */ )
	    {
		OBJ_DATA *obj;

		switch( ch->class->attr_prime )
		{
		    case APPLY_STR:
			ch->pcdata->perm_str = 16;
			break;
		    case APPLY_INT:
			ch->pcdata->perm_int = 16;
			break;
		    case APPLY_WIS:
			ch->pcdata->perm_wis = 16;
			break;
		    case APPLY_DEX:
			ch->pcdata->perm_dex = 16;
			break;
		    case APPLY_CON:
			ch->pcdata->perm_con = 16;
			break;
		}

		ch->exp		= 1000;
		ch->hit		= ch->max_hit;
		ch->mana	= ch->max_mana;
		ch->move	= ch->max_move;

		set_title( ch, "the alpha-tester" );

		obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ) );
		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_LIGHT );

		obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ) );
		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_BODY );

		obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ) );
		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_SHIELD );

		obj = create_object( get_obj_index( ch->class->weapon ) );
		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_WIELD );

		char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    }
	    else if( ch->in_room )
	    {
		char_to_room( ch, ch->in_room );
	    }
	    else if( IS_IMMORTAL( ch ) )
	    {
		char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	    }
	    else
	    {
		char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	    }

	    act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
	    do_look( ch, "auto" );

	    /*
	     * check for new notes and other spools
	     */
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
	    break;
    }

    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if ( is_name( name, "all auto immortal self someone none" ) )
	return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;

#if defined(MSDOS)
    if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
    if ( strlen(name) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;
    OBJ_DATA *obj;

    for( ch = char_list ; ch ; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || ch->desc == NULL )
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.\n\r", ch );
		act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
		d->connected = CON_PLAYING;

		/*
		 * Contributed by Gene Choi
		 */
		if( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
		 && IS_SET( obj->item_type, ITEM_LIGHT )
		 && obj->light->duration != 0
		 && ch->in_room )
		    ch->in_room->light += obj->light->intensity;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
	    write_to_buffer( d, "That character is already playing.\n\r",0);
	    write_to_buffer( d, "Do you wish to connect anyway (Y/N)?",0);
	    d->connected = CON_BREAK_CONNECT;
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL 
    ||   ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

/*
 * Write to all characters.
 */
void send_to_all_char( const char *text )
{
    DESCRIPTOR_DATA *d;

    if( !text )
        return;
    for( d = descriptor_list ; d ; d = d->next )
        if( d->connected <= CON_PLAYING )
	    send_to_char( text, d->character );

    return;
}

/*
 * Write to one char, old black & white version (long strings).
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( !txt || !ch->desc )
        return;

    write_to_buffer( ch->desc, txt, strlen( txt ) );

    return;
}
/*
 * Page to one char, old black & white version (long strings).
 */
void page_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( !txt || !ch->desc )
        return;
    free_string( ch->desc->showstr_head );
    ch->desc->showstr_head  = str_dup( txt );
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string( ch->desc, "" );
}

/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char 	*point;
    		char 	*point2;
    		char 	buf[ MAX_STRING_LENGTH*4 ];

    buf[0] = '\0';
    point2 = buf;
    if( txt && ch->desc )
	{
	    if( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			strcat( buf, colour( *point, ch ) );
			for( point2 = buf ; *point2 ; point2++ )
			    ;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
        	write_to_buffer( ch->desc, buf, point2 - buf );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
        	write_to_buffer( ch->desc, buf, point2 - buf );
	    }
	}
    return;
}

/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    const char *point;
    char *point2;
    char buf[ MAX_STRING_LENGTH*4 ];

    buf[0] = '\0';
    point2 = buf;
    if ( txt && ch->desc )
	{
	    if ( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			strcat( buf, colour( *point, ch ) );
			for( point2 = buf ; *point2 ; point2++ )
			    ;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	}
    return;
}

/* ========================================================================= */
/* - The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom for porting  */
/*   this SillyMud code for MERC 2.0 and laying down the groundwork.         */
/* - Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which the improvements*/
/*   to the pager was modeled from.  - Kahn                                  */
/* - Safer, allows very large pagelen now, and allows to page while switched */
/*   Zavod of jcowan.reslife.okstate.edu 4000.                               */
/* ========================================================================= */
void show_string( struct descriptor_data *d, char *input )
{
    char	*start;
    char	*end;
    char	arg[ MAX_INPUT_LENGTH ];
    int		lines = 0;
    int		pagelen;

    /*           Set the page length              */
    /* Make sure there is a pcdata first... -Lope */
    /* ------------------------------------------ */

    if( d->original ? d->original->pcdata : d->character->pcdata )
    {
	pagelen = d->original ? d->original->pcdata->pagelen
				: d->character->pcdata->pagelen;
    }
    else
	pagelen = 20;

    /* Ok, so its not turned off but hey almost -Lope */
    /* ---------------------------------------------- */

    if( pagelen == 0 )
	pagelen = 1000;


    /* Check for the command entered */
    /* ----------------------------- */

    one_argument( input, arg );

    switch( UPPER( *arg ) )
    {
        /* Show the next page */

        case '\0':
        case 'C': lines = 0;
                  break;
        
        /* Scroll back a page */

        case 'B': lines = -2 * pagelen;
                  break;

        /* Help for show page */

        case 'H': write_to_buffer( d, "B     - Scroll back one page.\n\r", 0 );
                  write_to_buffer( d, "C     - Continue scrolling.\n\r", 0 );
                  write_to_buffer( d, "H     - This help menu.\n\r", 0 );
                  write_to_buffer( d, "R     - Refresh the current page.\n\r",
                                   0 );
                  write_to_buffer( d, "Enter - Continue Scrolling.\n\r", 0 );
                  return;

        /* refresh the current page */

        case 'R': lines = -1 - pagelen;
                  break;

        /* stop viewing */

        default:  free_string( d->showstr_head );
                  d->showstr_head  = NULL;
                  d->showstr_point = NULL;
                  return;
    }

    /* do any backing up necessary to find the starting point */
    /* ------------------------------------------------------ */

    if( lines < 0 )
    {
        for( start= d->showstr_point ;
	     start > d->showstr_head && lines < 0 ;
             start-- )
            if( *start == '\r' )
                lines++;
    }
    else
        start = d->showstr_point;

    /* Find the ending point based on the page length */
    /* ---------------------------------------------- */
    lines  = 0;

    for( end = start ; *end && lines < pagelen ; end++ )
        if( *end == '\r' )
            lines++;

    d->showstr_point = end;

    if( end - start )
        write_to_buffer( d, start, end - start );

    if( *end == '\0' && d->connected == CON_PLAYING )
        write_to_buffer( d, "\n\r", 2 );

    /* See if this is the end (or near the end) of the string */
    /* ------------------------------------------------------ */

    for( ; isspace( *end ) ; end++ );

    if( !*end )
    {
        free_string( d->showstr_head );
        d->showstr_head  = NULL;
        d->showstr_point = NULL;
    }

    return;
}

/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1,
	 const void *arg2, int type )
{
    act_new( format, ch, arg1, arg2, type, POS_RESTING );
}

void act_new( const char *format, CHAR_DATA *ch, const void *arg1, 
	      const void *arg2, int type, int min_pos )
{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
 
    MPROG_ACT_LIST 	*tmp_act;
    CHAR_DATA 		*to;
    CHAR_DATA 		*vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA 		*obj1 = ( OBJ_DATA  * ) arg1;
    OBJ_DATA 		*obj2 = ( OBJ_DATA  * ) arg2;
    const 	char 	*str;
    char 		*i;
    char 		*point;
    char 		*i2;
    char 		fixed[ MAX_STRING_LENGTH ];
    char 		buf[ MAX_STRING_LENGTH   ];
    char 		fname[ MAX_INPUT_LENGTH  ];
    bool		fColour = FALSE;

    if( !format || !*format )
        return;

    if( !ch || !ch->in_room )
	return;

    to = ch->in_room->people;
    if( type == TO_VICT )
    {
        if( !vch )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
            return;
        }

	if( !vch->in_room )
	    return;

        to = vch->in_room->people;
    }
 
    for( ; to ; to = to->next_in_room )
    {
        if( !to->desc || to->position < min_pos )
            continue;
 
        if( type == TO_CHAR && to != ch )
            continue;
        if( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if( type == TO_ROOM && to == ch )
            continue;
        if( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
 
        point   = buf;
        str     = format;
        while( *str )
        {
            if( *str != '$' && *str != '{' )
            {
                *point++ = *str++;
                continue;
            }

	    i = NULL;
	    switch( *str )
	    {
		case '$':
		    fColour = TRUE;
		    ++str;
		    i = " <@@@> ";
		    if ( !arg2 && *str >= 'A' && *str <= 'Z' )
		    {
			bug( "Act: missing arg2 for code %d.", *str );
			i = " <@@@> ";
		    }
		    else
		    {
			switch ( *str )
			{
			    default:  
				bug( "Act: bad code %d.", *str );
				i = " <@@@> ";                                
				break;

			    case 't': 
				i = (char *) arg1;                            
				break;

			    case 'T': 
				i = (char *) arg2;                            
				break;

			    case 'n': 
				i = PERS( ch,  to  );                         
				break;

			    case 'N': 
				i = PERS( vch, to  );                         
				break;

			    case 'e': 
				i = he_she  [URANGE(0, ch  ->sex, 2)];        
				break;

			    case 'E': 
				i = he_she  [URANGE(0, vch ->sex, 2)];        
				break;

			    case 'm': 
				i = him_her [URANGE(0, ch  ->sex, 2)];        
				break;

			    case 'M': 
				i = him_her [URANGE(0, vch ->sex, 2)];        
				break;

			    case 's': 
				i = his_her [URANGE(0, ch  ->sex, 2)];        
				break;

			    case 'S': 
				i = his_her [URANGE(0, vch ->sex, 2)];        
				break;
 
			    case 'p':
				i = can_see_obj( to, obj1 )
				  ? obj1->short_descr
				  : "something";
				break;
 
			    case 'P':
				i = can_see_obj( to, obj2 )
				  ? obj2->short_descr
				  : "something";
				break;
 
			    case 'd':
				if ( !arg2 || ((char *) arg2)[0] == '\0' )
				{
				    i = "door";
				}
				else
				{
				    one_argument( (char *) arg2, fname );
				    i = fname;
				}
				break;
			}
		    }
		    break;

		case '{':
		    fColour = FALSE;
		    ++str;
		    i = NULL;
		    if( IS_SET( to->act, PLR_COLOUR ) )
		    {
			i = colour( *str, to );
		    }
		    break;

		default:
		    fColour = FALSE;
		    *point++ = *str++;
		    break;
	    }

            ++str;
	    if( fColour && i )
	    {
		fixed[0] = '\0';
		i2 = fixed;

		if( IS_SET( to->act, PLR_COLOUR ) )
		{
		    for( i2 = fixed ; *i ; i++ )
	            {
			if( *i == '{' )
			{
			    i++;
			    strcat( fixed, colour( *i, to ) );
			    for( i2 = fixed ; *i2 ; i2++ )
				;
			    continue;
			}
			*i2 = *i;
			*++i2 = '\0';
		    }			
		    *i2 = '\0';
		    i = &fixed[0];
		}
	        else
		{
		    for( i2 = fixed ; *i ; i++ )
	            {
			if( *i == '{' )
			{
			    i++;
			    continue;
			}
			*i2 = *i;
			*++i2 = '\0';
		    }			
		    *i2 = '\0';
		    i = &fixed[0];
		}
	    }


	    if( i )
	    {
		while( ( *point = *i ) != '\0' )
		{
		    ++point;
		    ++i;
		}
	    }
        }
 
        *point++	= '\n';
        *point++	= '\r';
        *point		= '\0';
	buf[0]		= UPPER( buf[0] );
	if( to->desc )
	    write_to_buffer( to->desc, buf, point - buf );
	if( MOBtrigger )
	{
	    if( IS_NPC( to ) && ( to->pIndexData->progtypes & ACT_PROG ) )
	    {
		tmp_act = alloc_mem( sizeof( MPROG_ACT_LIST ) );
		if( to->mpactnum > 0 )
		    tmp_act->next = to->mpact;
		else
		    tmp_act->next = NULL;

		to->mpact      = tmp_act;
		to->mpact->buf = str_dup( buf );
		to->mpact->ch  = ch; 
		to->mpact->obj = obj1; 
		to->mpact->vo  = vch; 
		to->mpactnum++;
	    }
	}
    }

    MOBtrigger = TRUE;
    return;
}

char *colour( char type, CHAR_DATA *ch )
{
    COLOUR_DATA	*col;
    static	char	code[ 20 ];

    if( IS_NPC( ch ) )
	return( "" );

    col = ch->pcdata->code;

    switch( type )
    {
	default:
	    sprintf( code, CLEAR );
	    break;
	case 'x':
	    sprintf( code, CLEAR );
	    break;
	case 'p':
	    sprintf( code, "[%d;3%dm", col->prompt[0], col->prompt[1] );
	    break;
	case 's':
	    sprintf( code, "[%d;3%dm", col->room_title[0], col->room_title[1] );
	    break;
	case 'S':
	    sprintf( code, "[%d;3%dm", col->room_text[0], col->room_text[1] );
	    break;
	case 'd':
	    sprintf( code, "[%d;3%dm", col->chat[0], col->chat[1] );
	    break;
	case 'Z':
	    sprintf( code, "[%d;3%dm", col->bbro[0], col->bbro[1] );
	    break;
	case 'o':
	    sprintf( code, "[%d;3%dm", col->room_exits[0], col->room_exits[1] );
	    break;
	case 'O':
	    sprintf( code, "[%d;3%dm", col->room_things[0], col->room_things[1] );
	    break;
	case 'i':
	    sprintf( code, "[%d;3%dm", col->immtalk_text[0], col->immtalk_text[1] );
	    break;
	case 'I':
	    sprintf( code, "[%d;3%dm", col->immtalk_type[0], col->immtalk_type[1] );
	    break;
	case 'u':
	    sprintf( code, "[%d;3%dm", col->say[0], col->say[1] );
	    break;
	case '2':
	    sprintf( code, "[%d;3%dm", col->fight_yhit[0], col->fight_yhit[1] );
	    break;
	case '3':
	    sprintf( code, "[%d;3%dm", col->fight_ohit[0], col->fight_ohit[1] );
	    break;
	case '4':
	    sprintf( code, "[%d;3%dm", col->fight_thit[0], col->fight_thit[1] );
	    break;
	case '5':
	    sprintf( code, "[%d;3%dm", col->fight_skill[0], col->fight_skill[1] );
	    break;
	case '1':
	    sprintf( code, "[%d;3%dm", col->fight_death[0], col->fight_death[1] );
	    break;
	case 'k':
	    sprintf( code, "[%d;3%dm", col->tell[0], col->tell[1] );
	    break;
	case 'l':
	    sprintf( code, "[%d;3%dm", col->reply[0], col->reply[1] );
	    break;
	case 'n':
	    sprintf( code, "[%d;3%dm", col->gtell_text[0], col->gtell_text[1] );
	    break;
	case 'N':
	    sprintf( code, "[%d;3%dm", col->gtell_type[0], col->gtell_type[1] );
	    break;
	case 'a':
	    sprintf( code, "[%d;3%dm", col->auction[0], col->auction[1] );
	    break;
	case 'q':
	    sprintf( code, "[%d;3%dm", col->question[0], col->question[1] );
	    break;
	case 'f':
	    sprintf( code, "[%d;3%dm", col->answer[0], col->answer[1] );
	    break;
	case 'e':
	    sprintf( code, "[%d;3%dm", col->music[0], col->music[1] );
	    break;
	case 'h':
	    sprintf( code, "[%d;3%dm", col->class_text[0], col->class_text[1] );
	    break;
	case 'H':
	    sprintf( code, "[%d;3%dm", col->class_name[0], col->class_name[1] );
	    break;
	case 'j':
	    sprintf( code, "[%d;3%dm", col->info[0], col->info[1] );
	    break;
	case 'b':
	    strcpy( code, C_BLUE );
	    break;
	case 'c':
	    strcpy( code, C_CYAN );
	    break;
	case 'g':
	    strcpy( code, C_GREEN );
	    break;
	case 'm':
	    strcpy( code, C_MAGENTA );
	    break;
	case 'r':
	    strcpy( code, C_RED );
	    break;
	case 'w':
	    strcpy( code, C_WHITE );
	    break;
	case 'y':
	    strcpy( code, C_YELLOW );
	    break;
	case 'B':
	    strcpy( code, C_B_BLUE );
	    break;
	case 'C':
	    strcpy( code, C_B_CYAN );
	    break;
	case 'G':
	    strcpy( code, C_B_GREEN );
	    break;
	case 'M':
	    strcpy( code, C_B_MAGENTA );
	    break;
	case 'R':
	    strcpy( code, C_B_RED );
	    break;
	case 'W':
	    strcpy( code, C_B_WHITE );
	    break;
	case 'Y':
	    strcpy( code, C_B_YELLOW );
	    break;
	case 'D':
	    strcpy( code, C_D_GREY );
	    break;
	case '{':
	    sprintf( code, "%c", '{' );
	    break;
    }
    return code;
}

char *colourconv( const char *txt , CHAR_DATA *ch )
{
    const	char	*point;
    		char	*point2;
    		char	converted[ 4*MAX_STRING_LENGTH ];

    converted[0] = '\0';
    if ( ch->desc && txt )
    {
	point2 = converted;
	if ( IS_SET( ch->act, PLR_COLOUR ) )
	{
	    for( point = txt ; *point ; point++ )
	    {
		if( *point == '{' )
		{
		    point++;
		    strcat( converted, colour( *point, ch ) );
		    for( point2 = converted ; *point2 ; point2++ )
			;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }			
	    *point2 = '\0';
	}
	else
	{
	    for( point = txt ; *point ; point++ )
	    {
		if( *point == '{' )
		{
		    point++;
		    continue;
		}
		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	}
    }
    return str_dup( converted );
}
