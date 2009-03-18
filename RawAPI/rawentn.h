/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 15.2.1996, Horst Goeringer
 *********************************************************************
 * rawentn.h
 *    entry definitions for client part of adsmcli client/server package
 *********************************************************************
 * 13. 4.1999, H.G.: mod. declaration of rawGetFileAttr
 * 31. 5.2001, H.G.: replace urmtopn by rconnect
 *  9. 1.2002, H.G.: created from rawent.h
 *                   rawGetLLName: pass object and delimiter as argument
 * 13. 2.2003, H.G.: mod arg list rawDelList
 * 14. 2.2003, H.G.: mod arg list rawCheckFilelist
 *********************************************************************
 */

#ifdef _AIX
unsigned long ielpst(unsigned long, unsigned long *);

int rawCheckAuth( char *, char *,
                  char *, char *, int,
                  char **);
   /* check if client authorized for requested action */
#endif

int rawCheckFilelist( char **, char **, char *);
   /* remove objects already archived from file list */

int rawCheckObjlist(int, char **, char **, char **);
   /* remove existing files from object and file list */

int rawDelFile( int, srawComm *);
   /* delete object in archive */

int rawDelList( int, srawDataMoverAttr *, srawComm *, char **, char **);
   /* delete list of objects in archive */

int rawGetCmdParms(int , char **, char **);
   /* get all cmd parameters via array of char ptrs */

#ifdef VMS
unsigned long rawGetFileAttr(char *, int **);
   /* get file attributes (reclen, size in bytes) */

int rawGetFilelist( char *, char **);
   /* get list of full file names from generic input */
#else
int rawGetFileAttr(char *, unsigned long *);
   /* get file attributes (size in bytes), return rc */

int rawGetFilelist( char *, int, char **);
   /* get list of full file names from generic input */
#endif

char *rawGetFSName( char *);
   /* get file space name from user identification */

char *rawGetFullFile(char *, char *);
   /* get full file name from generic input & ll name */

char *rawGetHLName( char *);
   /* get high level object name from path      */

int rawGetHostConn();
   /* get network connection type of client host */

int rawGetLLName( char *, char *, char *);
   /* get low level object name from file name    */

char *rawGetUserid();
   /* get user identification */

int rawQueryFile( int, srawComm *, void **);
   /* query for single object in archive */

void rawQueryPrint(srawObjAttr *, int);
   /* print query results for one object */

int rawQueryString(srawObjAttr *, int, char *);
   /* print query results for one object */

int rawRecvError( int, int, char **);
   /* receive error message */

int rawRecvHead( int, char **);
   /* receive common buffer header */

int rawRecvStatus( int, char **);
   /* receive common buffer header and error msg */

int rawSendStatus( int, int, char *);
   /* send status buffer */

int rawTapeFile( char *, char *, int, int *, unsigned long *);
   /* send status buffer */

int rawTestFilePath( char *, char **, char **);
   /* verify file path, get tape drive name */

#ifdef VMS
int rawCheckDevice (char *, char **, char **);
   /* check device type of specified file */

int rawGetTapeInfo( char *, char **, sTapeFileList **);
   /* get file names, sizes and buffer sizes from tape */

int rawSyncFilelist( char *, sTapeFileList **, int);
   /* synchronize both filelist buffers */

char *rawVMSName( char *, int);
   /* check if valid VMS name; if not change it to valid VMS name */

#endif

int rconnect( char *, int, int *, int *);
   /* open connection to specified server */



