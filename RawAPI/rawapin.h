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
 * created  1.12.2000 by Horst Goeringer
 *********************************************************************
 * rawapin.h
 *   entry definitions for client api
 * new version
 **********************************************************************
 *  7. 2.2001, H.G.: rfio_gsi_query added
 * 26. 2.2001, H.G.: rfio_fstat, rfio_perror, rfio_serror added
 *                   for ROOT version 300-05
 * 15.10.2002, H.G.: remove declaration of fLogFile
 * 13. 1.2003, H.G.: remove definition PORT_MOVER (in rawcommn.h)
 * 12. 6.2003, H.G.: add externals fLogFile, piFilelist
 **********************************************************************
 */

#define USER_API
#define RFILE srawAPIFile

#include "rawcommn.h"
#include "rawclin.h"

/* external in some functions */
FILE *fLogFile;
int* piFilelist;

extern RFILE *rfio_fopen( char *, char *);
   /* open file */

extern int rfio_fread( char *, int, int, RFILE *);
   /* read file buffer */

extern int rfio_fwrite( char *, int, int, RFILE *);
   /* write file buffer */

extern int rfio_fclose( RFILE *);
   /* close file */

extern int rfio_open( char *, int, int);
   /* open file */

extern int rfio_read(int, char *, int);
   /* read file buffer */

extern int rfio_write(int, char *, int);
   /* write file buffer */

extern int rfio_lseek(int, int, int);
   /* move read/write file mark */

extern int rfio_close(int);
   /* close file */

extern int rfio_access(char *, int);
   /* get access status */

extern int rfio_fstat(int, void*);
   /* get file status */

extern int rfio_parse(char *, char **, char **);
   /* parse name to host and path */

extern void rfio_perror(const char *);
   /* print error message */

extern char *rfio_serror();
   /* get error message */

extern int rfio_unlink(char *);
   /* */

extern int rfio_gsi_query(int, int, char *);
   /* get query info for remote file already opened */

