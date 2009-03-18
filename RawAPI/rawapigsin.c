/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 19. 3.2001 by Horst Goeringer
 ********************************************************************
 * rawapigsin.c
 *   GSI extensions for tsmcli client API (RFIO syntax)
 ********************************************************************
 * 19. 3.2001, H.G.: rfio_gsi_disconnect added
 *  3. 2.2003, H.G.: created from rawapigsi.c
 ********************************************************************
 */

#include <stdio.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include "rawcommn.h"
#include "rawapigsi.h"
#include "rawentn.h"

static int iint = sizeof(int);
FILE *fLogFile;

/*********************************************************************
 * rfio_gsi_disconnect.c
 *    close connection to specified server
 *********************************************************************
 */

int rfio_gsi_disconnect(int iSocket)               /* socket number */
{
   char cModule[32] = "rfio_gsi_disconnect";
   int iDebug = 0;
   int iRC = 0;
   int iStatus = STA_END_OF_SESSION;

   if (iSocket)
   {
      /* stop server process */
      iRC = rawSendStatus(iSocket, iStatus, NULL);
      if (iRC <= 0)
         printf("-E- sending status buffer (EOS)\n");
      else if (iDebug)
         printf("-D- status buffer (EOS) sent (%d bytes)\n", iRC);

      if (iDebug)
         printf("    close connection %d\n", iSocket);
      iRC = shutdown(iSocket, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSocket);
      if (iDebug)
         printf("    rc close = %d\n", iRC);

      return 0;
   }

   printf("-E- %s: invalid socket number specified: %d\n",
          cModule, iSocket);
   return -1;

} /* end rfio_gsi_disconnect */

