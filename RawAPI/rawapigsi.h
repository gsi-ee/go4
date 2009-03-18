/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 19. 3.2001 by Horst Goeringer
 *********************************************************************
 * rawapigsi.h
 *   entry definitions for client api: GSI extensions
 **********************************************************************
 * 19. 3.2001, H.G.: rfio_gsi_disconnect added
 **********************************************************************
 */

#define USER_API
#define RFILE srawAPIFile

extern int rfio_gsi_disconnect(int);
   /* close connection to remote server */

