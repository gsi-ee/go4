// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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
#else                 /* Unix */

#ifdef Lynx
#include <sys/types.h>
#else
#include <sys/types.h>
#endif

#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

#ifdef Linux
//#include <linux/in.h>
//#include <linux/inet.h>
#elif Lynx
#include <netinet/in.h>
#else               /* _AIX */
#include <in.h>
#endif

#endif              /* end Unix */

#if defined(VMS)
#include <inet.h>
#include <signal.h>
#include <unixio.h>
#endif

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

   if ( ( pHE = gethostbyname(cNode) ) == NULL )
   {
      lAddr = inet_addr(cNode);
      if ( ( pHE = gethostbyaddr(
               (char *)&lAddr, sizeof(lAddr), AF_INET ) ) == NULL )
      {
         fprintf(stderr,"-E- %s: unknown host %s\n", cModule, cNode );
         iError = -1;
         goto gError;
      }
      if (iDebug)
         printf("-D- %s: gethostbyaddr succeeded\n", cModule);
   }
   else if (iDebug)
      printf("-D- %s: gethostbyname succeeded\n", cModule);
   sHE = *pHE;                                        /* safe copy */

gRetryConnect:;

   /* create the socket */
   if ( ( iSocket = socket(
            AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == -1 )
   {
      perror("-E- rconnect(socket)");
      iError = errno;
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
      perror("-E- rconnect(bind)");
      iError = errno;
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
      /* if not successful, retry.  Possibly server not yet up. */
#ifdef WIN32
      if ( (iTime < iMaxTime) || (iMaxTime == -1) )
      {
         closesocket(iSocket);
         iSocket = -1;
#else
      if ( (errno == ECONNREFUSED) &&
           ( (iTime < iMaxTime) || (iMaxTime == -1) ) )
      {
         close(iSocket);
         iSocket = -1;
         if (imySigS) goto gError;            /* CTL C specified */
#endif

         if (iSleep < iMaxSleep) iSleep++;
         iTime += iSleep;
         if (iDebug)
            printf("    still trying (after %d sec) ...\n",iSleep);
#ifdef WIN32
         Sleep(iSleep*1000);
#else
         sleep(iSleep);
#endif
         goto gRetryConnect;
      }
#ifdef WIN32
      iError = -1;           /* errno, perror indicate NO error! */
#else
      perror("-E- rconnect(connect)");
      iError = errno;
#endif
      goto gError;
   }

   *piMaxTime = iTime;         /* return time needed for connect */
   *piSocket = iSocket;        /* return socket number */
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
   *piMaxTime = -1;
   return iError;
}

