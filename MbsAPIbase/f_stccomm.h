// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/*************************************************************************/
/* F_STCCOMM.H                                                           */
/* 18.7.95, H.G.: add message no. STC__ECONNREF                          */
/*************************************************************************/

#ifndef F_STCCOMM_H
#define F_STCCOMM_H

#include "typedefs.h" /* typedef INTS1, INTS2, ... */

#ifdef GSI__WINNT

#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <winsock.h>
#include <errno.h>
#define bzero(a,n) memset(a,0,n)

#endif
/* **************** WINDOWS_NT ********************************/


#ifdef VMS
INTS4 SYS$TRNLNM();
#endif

#ifdef _HPUX_SOURCE
#include <time.h>
#include </usr/include/sys/errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#endif

#ifdef VMS
#include <stdio.h>
#include <netdb.h>
#include <in.h>
#include <time.h>
#include <unixio.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
/*
 * The maximum number of file descriptors is now a configurable option
 * (max_nofile variable in /sys/conf/{mips|vax}/param.c).
 * The getdtablesize(2) system call should be used to obtain the
 * current limit. The value returned by getdtablesize() must be greater
 * than 64, and less than or equal to MAX_NOFILE in types.h . The
 * MAX_NOFILE define is needed for backward compatability with broken
 * programs that need a static sized array for selecting. These programs
 * should be modified to use the getdtablesize() interface for sizing.
 */
#define MAX_NOFILE   4096   /* This should not exist ! */
#define   NBBY   8      /* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef   FD_SETSIZE
#define   FD_SETSIZE   MAX_NOFILE
#endif   /* FD_SETSIZE */

/* How many things we'll allow select to use. 0 if unlimited */

#define MAXSELFD   MAX_NOFILE
/*typedef INTS4   fd_mask;*/
#define NFDBITS   (sizeof(fd_mask) * NBBY)   /* bits per mask (power of 2!)*/
#define NFDSHIFT 5            /* Shift based on above */
#ifndef howmany
#define   howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* howmany */

#define bzero(a,n) memset(a,0,n)
#define   FD_SET(n, p)   ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define   FD_CLR(n, p)   ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define   FD_ISSET(n, p)   ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)   bzero((INTS1 *)(p), sizeof(*(p)))

#endif /* VMS */

#ifdef OSK
#include <time.h>
#include <errno.h>
#include <types.h>
#include <stdio.h>
#include <socket.h>
#include <signal.h>
#include <in.h>
#include <netdb.h>

struct timeval {
    INTS4 tv_sec;
    INTS4 tv_usec;
};

#endif /* OSK */

#ifdef ultrix
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#endif

#ifdef Lynx
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <uio.h>
#endif

#ifdef GSI__LINUX
#undef unix
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#endif

#ifdef GSI__SOLARIS
#undef unix
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#endif


#ifdef _AIX
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#define _ALL_SOURCE                     /* for types.h typedef u_char...  */
#define _POSIX_SOURCE                   /*   any include is done          */

/*
 * The maximum number of file descriptors is now a configurable option
 * (max_nofile variable in /sys/conf/{mips|vax}/param.c).
 * The getdtablesize(2) system call should be used to obtain the
 * current limit. The value returned by getdtablesize() must be greater
 * than 64, and less than or equal to MAX_NOFILE in types.h . The
 * MAX_NOFILE define is needed for backward compatability with broken
 * programs that need a static sized array for selecting. These programs
 * should be modified to use the getdtablesize() interface for sizing.
 */
#define MAX_NOFILE   4096   /* This should not exist ! */
#define   NBBY   8      /* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef   FD_SETSIZE
#define   FD_SETSIZE   MAX_NOFILE
#endif   /* FD_SETSIZE */

/* How many things we'll allow select to use. 0 if unlimited */

#define MAXSELFD   MAX_NOFILE
typedef INTS4   fd_mask;
#define NFDBITS   (sizeof(fd_mask) * NBBY)   /* bits per mask (power of 2!)*/
#define NFDSHIFT 5            /* Shift based on above */
#ifndef howmany
#define   howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* howmany */
/* fd_set and FD_x are defined in time.h
typedef   struct fd_set {
   fd_mask   fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define bzero(a,n) memset(a,0,n)
#define   FD_SET(n, p)   ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define   FD_CLR(n, p)   ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define   FD_ISSET(n, p)   ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)   bzero((INTS1 *)(p), sizeof(*(p)))
*/
#endif /* AIX */

#ifdef unix   /* DEC OSF/1 */
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#define _ALL_SOURCE                     /* for types.h typedef u_char...  */
#define _POSIX_SOURCE                   /*   any include is done          */

/*
 * The maximum number of file descriptors is now a configurable option
 * (max_nofile variable in /sys/conf/{mips|vax}/param.c).
 * The getdtablesize(2) system call should be used to obtain the
 * current limit. The value returned by getdtablesize() must be greater
 * than 64, and less than or equal to MAX_NOFILE in types.h . The
 * MAX_NOFILE define is needed for backward compatability with broken
 * programs that need a static sized array for selecting. These programs
 * should be modified to use the getdtablesize() interface for sizing.
 */
#define MAX_NOFILE   4096   /* This should not exist ! */
#define   NBBY   8      /* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef   FD_SETSIZE
#define   FD_SETSIZE   MAX_NOFILE
#endif   /* FD_SETSIZE */

/* How many things we'll allow select to use. 0 if unlimited */

#define MAXSELFD   MAX_NOFILE
/*typedef INTS4   fd_mask;*/
#define NFDBITS   (sizeof(fd_mask) * NBBY)   /* bits per mask (power of 2!)*/
#define NFDSHIFT 5            /* Shift based on above */
#ifndef howmany
#define   howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* howmany */

/*typedef   struct fd_set {
   fd_mask   fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;*/

/*#define bzero(a,n) memset(a,0,n)*/
#define   FD_SET(n, p)   ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define   FD_CLR(n, p)   ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define   FD_ISSET(n, p)   ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
/*#define FD_ZERO(p)   bzero((INTS1 *)(p), sizeof(*(p)))*/

#endif /* unix */


struct s_tcpcomm {
        INTS4                socket  , sock_rw , namelength ;
        INTS4                i_signals;
        struct sockaddr_in sock , sock_name;
        struct hostent     hostentstruct;
        struct hostent     *hostentptr;
        INTS1              hostname[256] ;
        } ;

#ifdef vms
#define PORTSERV_DATA   "portserv.dat;1"
#else
#define PORTSERV_DATA   "portserv.dat"
#endif

#define MAXSERVER                    50
#define PORTSERVER_PORT              1000
#define PORTSERVER_M_DEF             "VSBP"
#define PORTSERVER_S_DEF             "DSAG"

struct s_comm_message {
        INTS1 c_message[128];
        INTS1 c_status[12];
        };

struct s_comm_portserv {
        INTS1    c_command[32];
        INTS1    c_serv_node[32];
        INTS1    c_serv_name[32];
        INTS1    c_date[32];
        INTS1    c_data[128];
        INTS1    c_port[8];
        } ;


#ifdef vms
#include <lnmdef.h>
#include <descrip.h>
#include <errno.h>
#include <socket.h>
#include <types.h>
#include <inet.h>
#include <time.h>
#include <ucx$inetdef.h>
#include <ssdef.h>
#include <iodef.h>
#define $DSC_FXDLEN(name,string) name.dsc$w_length = strlen(string); \
                                 strcpy(name.dsc$a_pointer,string);
#endif /* VMS */

#ifndef FALSE
#define FALSE                    0
#endif

#ifndef TRUE
#define TRUE                     1
#endif

#define f_stc_nosig( ps_server ) (ps_server)->i_signals = 100;

/* ********************************************************************** */
/*                   error messages                                       */
/* ********************************************************************** */

#define STC__FAILURE     0
#define STC__SUCCESS     1
#define STC__INVSOCK     2
#define STC__INVBUF      3
#define STC__NGBUFSIZE   4
#define STC__INVTIME     5
#define STC__TIMEOUT     6
#define STC__NOTSOCK     7
#define STC__INVADDR     8
#define STC__INVADDRF    9
#define STC__SOCKNSUP   10
#define STC__INVPROTO   11
#define STC__SOCKTABF   12
#define STC__SOCKSPAF   13
#define STC__SOCKISC    14
#define STC__CONNTOUT   15
#define STC__NETUNREA   16
#define STC__PORTINUS   17
#define STC__SOCKISP    18
#define TPS__ECPORTS    19
#define TPS__EREGSERV   20
#define TPS__EWTOPORTS  21
#define TPS__ERMFRPORTS 22
#define TPS__EGSERVICE  23
#define STC__BROPIPE    24
#define STC__NODATA     25
#define STC__ECONNREF   26
#define STC__EINTR      27
#define STC__ECONNRES   28

#define TCPCOMM_INCLUDE   1


#ifndef OSK
INTS4 f_stc_connectserver (CHARS *c_node, INTS4 l_port, INTS4 *pi_channel, struct s_tcpcomm *ps_client);
INTS4 f_stc_createserver  (INTS4 *pl_port, struct s_tcpcomm *ps_server);
INTS4 f_stc_listenserver  (struct s_tcpcomm *ps_server);
INTS4 f_stc_acceptclient  (struct s_tcpcomm *ps_server, INTS4 *pi_channel);
INTS4 f_stc_disperror     (INTS4 i_error, CHARS *c_dest, INTS4 i_out);
INTS4 f_stc_read          (void *p_buffer, INTS4 i_buflen, INTS4 i_channel , INTS4 i_timeout);
INTS4 f_stc_write         (void *p_buffer, INTS4 i_buflen, INTS4 i_channel);
INTS4 f_stc_close         (struct s_tcpcomm *ps_tcp);
INTS4 f_stc_discclient    (INTS4 i_channel);

void  f_stc_swap4  (INTS4 *);

#endif /* OSK */

#endif
