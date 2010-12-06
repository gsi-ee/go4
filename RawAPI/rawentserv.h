/**********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 15.2.1996 by Horst Goeringer
 **********************************************************************
 * rawentserv.h
     entry definitions for server part of GSI mass storage system
 **********************************************************************
 */

 char * rawGetLogName( char *);
    /* get log file name for adsmcli server process */

 /* int rawRetrieve(srawStageComm *); */
    /* handle retrieve actions via storage manager API */

 int rawStageFileName(char *, char *, char *, char *);
    /* convert ADSM representation to relative stage file name */

 int rawIsStaged(char *, char *, char *, int *, char *, char *, char *);
    /* check if object already in staging pool */

 /* end rawentserv.h */
