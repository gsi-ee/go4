/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt  
 *   Germany
 * created 26. 1.1996 by Horst Goeringer
 *********************************************************************
 * rconnect.c
 * open connection to specified server 
 *********************************************************************
 *  7. 4.1999, H.G.: return error number in case of failure
 *                   pass max time for retries of connect()
 *  3. 3.2000, H.G.: rename iExit to imySigS
 *                   *piMaxTime: meaning of values -1 and 0 exchanged
 *  7. 3.2000, H.G.: renamed form rawConnect to rconnect
 * 31.10.2001, H.G.: ported to W2000
 * 14.10.2002, H.G.: ported to Lynx
 *  1. 2.2005, H.G.: ported to Linux and gcc322
 *  7.10.2008, H.G.: improved retry loops
 *  3.11.2008, H.G.: correct include files for AIX
 *  5.11.2010, H.G.: replace perror by strerror(errno)
 *********************************************************************
 */  
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32          /* Windows */
#include <sys\types.h>
#include <winsock.h>  
#include <windows.h>  
#include <process.h>  
#else                 /* all Unix */
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#ifdef Lynx           /* Lynx */
#include <netinet/in.h>
#include <socket.h>
#elif Linux          /* Linux */
#include <arpa/inet.h>
#include <sys/socket.h>
#else               /* AIX */
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#endif              /* all Unix */

extern int imySigS;                      /* if = 1: CTL C specified */

/********************************************************************/

int rconnect( char *cNode,        /* input:  name of remote host */
              int iPort,          /* input:  port number */
              int *piMaxTime,     /* input:  max time for connect */
                                  /* = -1: try only once */
                                  /* =  0: try until success */
                                  /* >  0: try at most *piMaxTime s */
                                  /* output: time needed for connect */
              int *piSocket)      /* output: socket number */
{
   char cModule[32] = "rconnect";
   int iDebug = 0; 
   int iSocket = 0; 
   int iMaxTime = *piMaxTime; 
   int iTime = 0; 
   int iError;
   int iSleep = 0;
   int iMaxSleep = 10;             /* max sleep time */

   struct hostent *pHE = NULL;
   struct hostent  sHE;
   struct sockaddr_in sSockAddr;
   unsigned long lAddr;

   if (iDebug)
   {
      printf("\n-D- begin %s: try connection to %s:%d",
         cModule, cNode, iPort); 
      if (iMaxTime < 0)
         printf(" (1 trial)\n");
      else if (iMaxTime == 0)
         printf(" (until success)\n");
      else
         printf(" (for %d sec)\n", iMaxTime);
   }

   if ( ( pHE = gethostbyname(cNode) ) == NULL ) 
   {
      lAddr = inet_addr(cNode);
      if ( ( pHE = gethostbyaddr( 
               (char *)&lAddr, sizeof(lAddr), AF_INET ) ) == NULL )
      {
         printf("-E- %s: unknown host %s\n", cModule, cNode );
         iError = -1;
         goto gError;
      }
      if (iDebug) 
         printf("    %s: gethostbyaddr succeeded\n", cModule);
   }
   else if (iDebug) 
      printf("    %s: gethostbyname succeeded\n", cModule);
   sHE = *pHE;                                        /* safe copy */

gRetryConnect:;
   /* create the socket */
   if ( ( iSocket = socket( 
            AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 )
   {
      printf("-E- %s: socket failed\n", cModule);
      if (errno)
      {
         printf("    %s\n", strerror(errno));
         errno = 0;
      }

      iError = -1;
      goto gError;
   }
 
   sSockAddr.sin_family = AF_INET;
   sSockAddr.sin_port   = 0;
   sSockAddr.sin_addr.s_addr = INADDR_ANY;
                                 /* bind a socket */
   if ( bind ( iSocket,
               (struct sockaddr *) &sSockAddr,
               sizeof(sSockAddr) ) == -1 )
   {
      printf("-E- %s: bind failed\n", cModule);
      if (errno)
      {
         printf("    %s\n", strerror(errno));
         errno = 0;
      }

      iError = -1;
      goto gError;
   }
 
   sSockAddr.sin_family = AF_INET;
   sSockAddr.sin_port   = htons( (short) iPort );
   sSockAddr.sin_addr = * ( (struct in_addr *) sHE.h_addr );
                                 /* contact remote server */
   if ( connect( iSocket,
                 (struct sockaddr *) &sSockAddr,
                 sizeof(sSockAddr) ) == -1 )
   {
#ifdef WIN32
      iError = -1;           /* errno, perror indicate NO error! */
#else
      printf("-E- %s: connect to %s:%d failed\n", cModule, cNode, iPort);
      if (errno)
      {
         printf("    %s\n", strerror(errno));
         errno = 0;
      }
#endif

      /* if not successful, retry.  Possibly server not yet up. */
      if ( (iTime < iMaxTime) || (iMaxTime == -1) )
      {
#ifdef WIN32
         closesocket(iSocket);
         iSocket = -1;
#else
         close(iSocket); 
         iSocket = -1;
         if (imySigS)                         /* CTL C specified */
            goto gError;
#endif

         if (iMaxTime == -1)                   /* only one trial */
            goto gError;

         if (iSleep < iMaxSleep)
            iSleep++;
         iTime += iSleep;

         if (iDebug) printf(
            "    time %d of %d, sleep %d\n",
            iTime, iMaxTime, iSleep);
#ifdef WIN32
         Sleep(iSleep*1000);
#else
         sleep((unsigned) iSleep);
#endif
         goto gRetryConnect;
      }

      goto gError;
   }

   *piMaxTime = iTime;         /* return time needed for connect */
   *piSocket = iSocket;        /* return socket number */
   if (iDebug)
      printf("-D- end %s (success after %d sec)\n\n", cModule, *piMaxTime); 

   return(0);                  /* no error */

gError: 
   if (iSocket)
   {
      shutdown(iSocket, 2);
#ifdef WIN32
      closesocket(iSocket);
#else
      close(iSocket);
#endif
   }

   *piSocket = -1;
   if (iMaxTime >= 0)
      *piMaxTime = iTime;          /* return time needed for trials */
   else
      *piMaxTime = 0;

   if (iDebug)
   {
      printf("-D- end %s", cModule); 
      if (iMaxTime >= 0)
         printf(" (after %d sec)\n\n", iTime); 
      else
         printf("\n\n"); 
   }

   return iError;
}
