#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE	4096
#define SIZ	500

extern	int	errno;
char	*auth_tcpuser(	register long unsigned int in,
			register short unsigned int local,
			register short unsigned int remote );
unsigned short auth_tcpport = 113;

void err_sys( const char *str )
{
    FILE	*log_file;

    if( !( log_file = fopen( "./getnameauth_err.log", "a+t" ) ) )
    {
	perror( "can't open Error log!" );
	return;
    }    
    fprintf( log_file, "%s\n", str );
    fclose( log_file );

    return;
}

int main( void ) 
{	
    int		n;
    int		int1;
    int		int2;
    int		addr;
    int		server_port;
    struct	hostent	*from;
    char	*user;
    char	line[ MAXLINE ];
    char	addr_buff[ 64 ];
    char	name_buff[ 64 ];

    while( ( n = read( STDIN_FILENO, line, MAXLINE ) ) > 0 )
    {
	line[ n ] = '\0';
	if( sscanf( line, "%s %d %d %d",
	    &name_buff[ 0 ], &int1, &int2, &server_port ) == 4 )
	{
	    from = gethostbyaddr( ( char* )&int1, sizeof( int1 ), AF_INET );     
	    user = auth_tcpuser( int1, server_port, int2 );
	    addr = ntohl( int1 );

	    sprintf( addr_buff, "%d.%d.%d.%d port %d",
		( addr >> 24 ) & 0xFF,
		( addr >> 16 ) & 0xFF,
		( addr >>  8 ) & 0xFF,
		( addr       ) & 0xFF,
		int2 );

	    sprintf( line, "%s:%s:%s",
		name_buff,
		user,
		from ? from->h_name : addr_buff );

	    n = strlen( line );
	    if( write( STDOUT_FILENO, line, n ) != n )
		err_sys( "Error writing to pipe" );
	}
	else
	{
	    if( write( STDOUT_FILENO, "Invalid Query\n", 14 ) != 14 )
		err_sys( "Error writing to pipe" );
	}
    }
    exit( 0 ); 
}


static	char	ruser[ SIZ ];
static	char	realbuf[ SIZ ];
static	char	*buf;

/* This was taken from public domain source that I managed to dredge up,
 * I changed a little of it, but I can't claim that I wrote it all.  I don't
 * know who the original author is, so I can't give him/her credit. */ 

char *auth_tcpuser(
	register long unsigned int in,
	register short unsigned int local,
	register short unsigned int remote )
{
    struct	sockaddr_in	sa;
    register	int		s;
    register	int		buflen;
    register	int		w;
    register	int		saveerrno;
		char		ch;
    unsigned	short		rlocal;
    unsigned	short		rremote;

    if( ( s = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	return 0;

    sa.sin_family	= AF_INET;
    sa.sin_port		= htons( auth_tcpport );
    sa.sin_addr.s_addr	= in;

    if( connect( s, ( struct sockaddr * )&sa, sizeof( sa ) ) == -1 )
    {
	saveerrno = errno;
	close( s );
	errno = saveerrno;
	return( "(Unknown)" );
    }
    buf	= realbuf;
    sprintf( buf, "%u , %u\r\n",
	( unsigned int )remote,
	( unsigned int )local );

    /*
     * note the reversed order, the example in the RFC is misleading
     */
    buflen = strlen( buf );
    while( ( w = write( s, buf, buflen ) ) < buflen )
	if( w == -1 )	/* should we worry about 0 as well? */
	{
	    saveerrno = errno;
	    close( s );
	    errno = saveerrno;
	    return( "(Send Error)" );
	}
	else
	{
	    buf += w;
	    buflen -= w;
	}

    buf = realbuf;
    while( ( w = read( s, &ch, 1 ) ) == 1 )
    {
	*buf = ch;
	if( ( ch != ' ' )
	 && ( ch != '\t' )
	 && ( ch != '\r' ) )
	    ++buf;
	if( ( buf - realbuf == sizeof( realbuf ) - 1 )
	 || ( ch == '\n' ) )
	    break;
    }
    if( w == -1 )
    {
	saveerrno = errno;
	close( s );
	errno = saveerrno;
	return( "(Read Error)" );
    }
    *buf = '\0';
    bzero( ruser, SIZ );
    if( sscanf( realbuf, "%hd,%hd: USERID :%*[^:]:%s",
		    &rremote, &rlocal, ruser ) < 3 )
    {
	close( s );
	errno = EOF;
	/*
	 * Makes sense, right? well, not when USERID failed to match ERROR
	 * but there's no good error to return in that case
	 */
	fprintf( stderr, realbuf );
	return( "(Auth Error)" );
    }
    if( ( remote != rremote )
     || ( local != rlocal ) )
    {
	close( s );
	errno = EOF;
	return( "REMOTE/LOCAL" );
    }
    close( s );

    return ruser;
}

