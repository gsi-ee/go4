/**********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 14. 2.1996, Horst Goeringer 
 **********************************************************************
 * rawProcn.c 
 *    utility programs for gStore package: client and server
 **********************************************************************
 * rawAddStrings:    add 2 messages avoiding overflows
 * rawCheckClientFile: check gStore naming conventions for client file
 * rawGetFSpName:    get file space name from user specification
 * rawGetHLName:     get high level object name from path
 * rawGetLLName:     get low level object name from file name
 * rawGetFileSize:   get file size (byte) via Posix call stat
 * rawGetPathName:   get path name from high level object name
 * rawQueryFile:     get query information for single file
 * rawRecvError:     receive error message
 * rawRecvHead:      receive common buffer header
 * rawRecvHeadC:     receive common buffer header and check
 * rawRecvRequest:   receive request for next buffer
 * rawRecvStatus:    receive status buffer
 * rawSendRequest:   send request for next buffer
 * rawSendStatus:    send status buffer
 * rawTestFileName:  verify that specified name is a valid file name
 **********************************************************************
 * 23. 4.1996., H.G: rawGetHLName: remove device dependency in hl-name,
 *                   as in ADSM 213 class may be set in dsmSendObj
 * 16. 5.1997, H.G.: keep only modules for all clients here
 * 16. 6.1997, H.G.: ported to Linux
 * 15.10.1997, H.G.: rawCheckAuth moved to rawCheckAuth.c
 * 22.10.1997, H.G.: rawDelFilelist added
 *  8. 7.1998, H.G.: new filetype STREAM
 * 19.10.1998, H.G.: rawapitd.h introduced, dsmapitd.h removed
 *  1.12.2000, H.G.: rawSendStatus: send also buffer request headers
 *  7. 2.2001, H.G.: new function rawQueryString
 * 21. 2.2001, H.G.: renamed file rawQueryFile.c -> rawProcQuery.c
 *                   function rawQueryList added
 * 18. 5.2001, H.G.: function rawDelFilelist -> rawcliproc.c
 *                   merge into new file rawcliproc.c: rawProcQuery.c
 * 18. 6.2001, H.G.: function rawTestFile: renamed to  rawTestFileName,
 *                   handle mixed case
 *  2.11.2001, H.G.: ported to W2000
 * 14.11.2001, H.G.: take alternate delimiter for Windows into account
 *  9. 1.2002, H.G.: rawGetLLName: pass object and delimiter as argument
 * 19. 2.2002, H.G.: rawGetFileSize added
 * 20. 2.2002, H.G.: rawGetCmdParms removed (unused)
 *  7. 3.2002, H.G.: rawQueryPrint: new type GSI_MEDIA_INCOMPLETE
 *                   rawRecvHeadC added
 *  7. 6.2002, H.G.: rawRecvHeadC: check always for error status
 * 23. 7.2002, H.G.: replace old printf() -> fprintf(fLogFile)
 *  5. 8.2002, H.G.: rawQueryFile: handle QUERY_RETRIEVE, QUERY_STAGE
 * 14.10.2002, H.G.: ported to Lynx
 * 31. 1.2003, H.G.: use rawdefn.h
 * 17. 2.2003, H.G.: rawCheckFilelist, rawCheckObjlist -> rawcliprocn.c
 *  3. 6.2003, H.G.: renamed from rawprocn to rawProcn
 *  9. 7.2003, H.G.: rawTestFileName: call rawGetFileSize to identify
 *                   directories
 * 13. 8.2003, H.G.: rawQueryPrint: add user who staged file in output
 *  5.12.2003, H.G.: rawRecvStatus: convert status header to host format
 * 28. 1.2004, H.G.: GSI_MEDIA_CACHE: rawQueryPrint, rawQueryString
 *                   GSI_MEDIA_INCOMPLETE: in rawQueryString
 * 26. 5.2004, H.G.: rawQueryPrint, rawQueryString: new print modes:
 *                   stage status not checked
 * 29. 6.2004, H.G.: rawRecvHeadC: recv error msg, if write cache full
 * 24.11.2004, H.G.: rawQueryFile: handle status values
 *                   STA_NO_ACCESS, STA_ARCHIVE_NOT_AVAIL
 * 25. 1.2005, H.G.: use rawapitd-gsin.h, new media names:
 *                   GSI_..._LOCKED, GSI_..._INCOMPLETE
 *  1. 2.2005, H.G.: ported to Linux and gcc322
 * 25.11.2005, H.G.: rename rawGetFSName to rawGetFSpName (file space),
 *                   rawGetFSName in rawProcPooln (get file system name)
 *  3. 4.2006, H.G.: ported to sarge
 * 29. 9.2006, H.G.: rawQueryPrint: handle ATL server
 *  4.10.2006, H.G.: rawQueryPrint: handle cacheDB entry V4
 *  2.11.2006, H.G.: rawQueryString: handle ATL server, cacheDB entry V4
 *  8.12.2006, H.G.: rawQueryFile: handle query on several ATL Servers
 *  8. 5.2008, H.G.: rawGetFullFile: move -> rawCliProcn.c
 *                   rawQueryFile: moved from rawCliProcn.c, repl printf
 *                   rawQueryList: moved from rawCliProcn.c, repl printf
 * 16. 5.2008, H.G.: rawGetHLName: remove './' and trailing '/.' in path
 * 28. 8.2008, H.G.: rawQueryPrint, rawQueryString:
 *                   remove ATL server no. in query output
 *  6.10.2008, H.G.: replace perror by strerr(errno), improve msgs
 *                   rawRecvRequest: better handling rc=0 from recv()
 * 30.10.2008, H.G.: rawQueryFile,rawQueryList: better: rc=0 from recv()
 *  5.11.2008, H.G.: rawRecvError: better handling rc=0 from recv()
 *                      arg 3: char ** -> char *
 *                   rawRecvHead: better handling rc=0 from recv()
 *                      arg 3: char ** -> char *
 *                   rawRecvStatus: better handling rc=0 from recv()
 *                      arg 3: char ** -> char *
 * 11.11.2008, H.G.: add suggestions of Hakan
 *  3.12.2008, H.G.: add suggestions of Hakan, part II
 * 12.12.2008, H.G.: handle CUR_QUERY_LIMIT
 *  4. 5.2009, H.G.: rawRecvHeadC: improve debug output
 *  7. 5.2009, H.G.: new entry rawAddStrings
 * 22. 6.2008, H.G.: replace long->int if 64bit client (ifdef SYSTEM64)
 *  8. 7.2008, H.G.: rawQueryPrint/String: print filesize with %10d
 * 23.11.2009, H.G.: rawTestFileName: inhibit '*', '?' as file name part
 * 10.12.2009, H.G.: rawQueryFile, rawQueryPrint, rawQueryString:
 *                      handle enhanced structure srawObjAttr
 *                      old version info also available in new structure
 *                   rawQueryList not upgraded, obviously unused???
 * 29. 1.2010, H.G.: replace MAX_FILE -> MAX_FULL_FILE in:
 *                      static string cPath,
 *                      rawTestFilePath: cTemp
 *                   enhance cMsg: 256 -> STATUS_LEN in:
 *                      rawRecvHeadC, rawRecvRequest
 *  5. 2.2010, H.G.: move rawQueryPrint,rawQueryString -> rawCliProcn.c
 *                   move rawQueryList -> Unused.c
 * 11. 2.2010, H.G.: rename rawTestFilePath -> rawCheckClientFile
 *                   rawCheckClientFile: all checks of local file name
 *                      concerning gStore conventions are done here
 *  3. 5.2010, H.G.: rawGetFileSize: specific error msg if filesize>2GB
 * 23. 8.2010, H.G.: rawGetFileSize, rawTestFileName:
 *                   remove SYSTEM64, allow "long"
 *  5.11.2010, H.G.: reset errno after error,
 *                   better error handling after recv/send
 * 18.11.2010, H.G.: rawRecvError: provide also incomplete msg
 **********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32          /* Windows */
#include <sys\types.h>
#include <winsock.h>
#include <windows.h>
#include <process.h>
#include <sys\stat.h>
#else                 /* all Unix */
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
   
#ifdef Lynx           /* Lynx */
#include <sys/types.h>
#include <socket.h>
#elif Linux           /* Linux */
#include <sys/socket.h>
#else                 /* AIX */
#include <sys/socket.h>
#endif

#endif              /* all Unix */

#include "rawapitd.h"
#include "rawapitd-gsin.h"
#include "rawcommn.h"
#include "rawdefn.h"
#include "rawentn.h"
#include "rawapplcli.h"

extern FILE *fLogFile;

static char cPath[MAX_FULL_FILE] = "";
static char cNamefs[MAX_OBJ_FS] = "";
static char cNamehl[MAX_OBJ_HL] = "";
static char *pcNull = (char *) "";/* to be returned in case of error */
                        /* return(NULL) works in AIX, but not in VMS */

static int iObjAttr = sizeof(srawObjAttr);

/*********************************************************************
 * rawAddStrings
 *    add 2 messages avoiding string overflows
 * created 7.5.2009, Horst Goeringer
 *********************************************************************
 */

int rawAddStrings(char *pcMsg1,       /* 1st string, always complete */
                  int iMaxLength1,          /* max length of string1 */
                  char *pcMsg2,        /* added string, possibly cut */
                  int iMaxLength2,          /* max length of string2 */
                  int iErrno,   /* = 0: add contents of string2 
                                   > 0: add system error msg (errno) */
                  char *pcMsgE,      /* error msg in case of problem */
                  int iMaxLengthE)     /* max length of error string */
{
   char cModule[32]="rawAddStrings";
   int iDebug = 0;

   int iRC = 0;   /* = 0: okay 
                     < 0: problem:
                        =-1: specified string too large
                        =-2: string without contents provided 
                        =-3: max length <=0 provided 
                        =-4: iErrno, but string2 < STATUS_LEN byte
                        =-5: error string < STATUS_LEN byte provided 
                     > 0: no. of chars cut from the 2nd string */
   int iString1 = 0;
   int iString2 = 0;
   int iStringE = 0;
   int iString = 0;                     /* sum of string1 + string2 */
   int iMinLen = STATUS_LEN;               /* for pcMsgE, if needed */
   char cMsg[STATUS_LEN] = "";                     /* internal copy */
   char *pcc;

   iString1 = strlen(pcMsg1);
   iString2 = strlen(pcMsg2);
   iString = iString1 + iString2;

   if (iDebug)
   {
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);
      fprintf(fLogFile, "    string1 (%d byte, max %d byte):\n    %s",
         iString1, iMaxLength1, pcMsg1);
      if (iErrno)
         fprintf(fLogFile, "    add msg for errno=%d\n", errno);
      else
         fprintf(fLogFile, "    add string2 (%d byte, max %d byte):\n    %s",
            iString2, iMaxLength2, pcMsg2);
   }

   if ( (iString1 <= iMaxLength1) && 
        (iString2 <= iMaxLength2) )
   {
      if (iErrno)
      {
         if (iMaxLength2 >= iMinLen)
         {
            sprintf(pcMsg2, "    %s\n", strerror(errno));
            iString2 = strlen(pcMsg2);
            iString = iString1 + iString2;
         }
         else
         {
            iRC = -4;
            sprintf(cMsg,
               "-E- %s: 2nd string (%d byte) possibly too small for system error: requ. %d byte\n",
               cModule, iMaxLength2, iMinLen);

            goto gErrorMsg;
         }
      } /* (iErrno) */

      if (iString < iMaxLength1)
      {
         strcat(pcMsg1, pcMsg2);
         if (iDebug) fprintf(fLogFile,
            "    returned msg (%d byte):\n    %s", iString, pcMsg1);
      }
      else
      {
         strncat(pcMsg1, pcMsg2, iMaxLength1-iString1-1);
         strncat(pcMsg1, "\n", 1);
         iRC = iString -iMaxLength1;
         if (iDebug)
         {
            fprintf(fLogFile, "    returned msg:\n    %s", pcMsg1);
            pcc = pcMsg2;
            pcc+= iMaxLength1-iString1-1;
            fprintf(fLogFile, "    skipped msg part:\n    %s", pcc);
         }
      }
   }
   else
   {
      if (iMaxLength1 <= 0)
      {
         iRC = -3;
         sprintf(cMsg, "-E- %s: 1st string length (%d byte) invalid\n",
            cModule, iMaxLength1);
      }
      if (iMaxLength2 <= 0)
      {
         iRC = -3;
         sprintf(cMsg, "-E- %s: 2nd string length (%d byte) invalid\n",
            cModule, iMaxLength2);
      }
      else if (iString == 0)
      {
         iRC = -2;
         sprintf(cMsg, "-E- %s: empty strings provided\n", cModule);
      }
      else if (iString1 > iMaxLength1)
      {
         iRC = -1;
         sprintf(cMsg, "-E- %s: 1st string longer (%d byte) than length provided (%d)\n",
            cModule, iString1, iMaxLength1);
      }
      else if ( (iString2 > iMaxLength2) && (iErrno == 0) )
      {
         iRC = -1;
         sprintf(cMsg, "-E- %s: 2nd string longer (%d byte) than length provided (%d)\n",
            cModule, iString2, iMaxLength2);
      }
   }

   if (iRC >= 0)
   {
      strcpy(pcMsgE, "");
      goto gEndAddStrings;
   }

gErrorMsg:
   iStringE = strlen(cMsg);
   if (iDebug) fprintf(fLogFile,
      "    error string (%d byte, max %d byte):\n    %s\n",
      iStringE, iMaxLengthE, cMsg);

   if (iMaxLengthE < iStringE)
   {
      iRC = -5;
      fprintf(fLogFile, "-E- %s: size error string too small: max %d byte (needed %d)",
         cModule, iMaxLengthE, iStringE);
      strncat(pcMsgE, cMsg, iMaxLengthE);
      fprintf(fLogFile, ", provided: %s\n", pcMsgE);
   }
   else
   {
      if (iDebug)
         fprintf(fLogFile, cMsg);
      strcpy(pcMsgE, cMsg);
   }

gEndAddStrings:
   if (iDebug)
      fprintf(fLogFile, "-D- end %s: rc=%d\n\n", cModule, iRC);

   return iRC; 

} /* rawAddStrings */

/********************************************************************
 * rawCheckClientFile:
 *    check client file name for gStore naming conventions
 *    verify that file path not generic
 *    if tape specified get tape drive name
 *    returns 0 if valid disk file name
 *    returns >0 if valid tape file name
 *
 * created 15. 3.96, Horst Goeringer
 ********************************************************************
 */

int rawCheckClientFile( char *pcFile0, char **pcFile, char **pcTape)
{

   char cModule[32]="rawCheckClientFile";
   int iDebug = 0;

   int iloc, iRC, ii;
   int iDevType = 1;                     /* default: disk */

   bool_t bup = bFalse;

   char cText[12] = "";
   char cTemp[MAX_FULL_FILE] = "";
   char *plocd = NULL, *plocs = NULL;
   char *pdelim = NULL, *pgen = NULL;
   const char *pcSemi = ";";
   char *pcc = NULL, *pBufTape, *pBufFile;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: test file %s\n", cModule, pcFile0);

   pBufTape = *pcTape;
   pBufFile = *pcFile;
   strcpy(cTemp, pcFile0);

   /* check if device name specified */
   plocd = strchr(cTemp, *pcDevDelim);
   if (plocd != NULL)
   {
      iRC = strncmp(cTemp, "/dev", 4);
      {
         fprintf(fLogFile,
            "-E- invalid file name: %s\n    path names must not contain '%s'\n",
            pcFile0, pcDevDelim);
         return(-2);
      }

      if (iRC)
      pdelim = strrchr(cTemp, *pcFileDelim);
      if ( (pdelim == NULL) || (pdelim > plocd) )
      {
         fprintf(fLogFile,
            "-E- invalid disk file name: %s\n    path names must not contain '%s'\n",
            pcFile0, pcDevDelim);
         return(-2);
      }

      pcc = plocd;
      pcc++;
      if (strlen(pcc) > MAX_TAPE_FILE)
      {
         ii = MAX_TAPE_FILE;
         fprintf(fLogFile,
            "-E- invalid tape file name: %s\n    name too long: max %d byte after dev name\n",
            pcFile0, ii);
         return(-2);
      }

      iDevType = 2;        /* tape, if Unix; if VMS: get it later */
      iRC = strncmp(++pcc, "\0", 1);
      if (iRC == 0)
      {
#ifdef VMS
         fprintf(fLogFile,
            "-E- file name must be specified explicitly\n");
#else
         fprintf(fLogFile, "-E- file name missing in %s\n", pcFile0);
#endif
         return(-2);
      }
   } /* (plocd != NULL) */

#ifdef VMS
   /* eliminate version number */
   plocs = strchr(cTemp, *pcSemi);
   if (plocs)
   {
      ++plocs;
      iRC = isalnum(*plocs);
      if (iRC)                      /* char after semicolon alphanum. */
      {
         iRC = isalpha(*plocs);
         if (iRC == 0)              /* char after semicolon no letter */
            fprintf(fLogFile,
               "-W- version number in file specification removed\n");
         else
            fprintf(fLogFile,
               "-W- invalid version in file specification removed\n");
      }
      else
         fprintf(fLogFile,
            "-W- semicolon in file specification removed\n");
      strncpy(--plocs, "\0", 1);
      strcpy(pcFile0, cTemp);
   }

   iDevType = rawCheckDevice(cTemp, pcFile, pcTape);
   switch(iDevType)
   {
      case 0:
         if (iDebug) fprintf(fLogFile,
            "    no device name specified\n");
         break;
      case 1:
         if (iDebug)
         {
            if (strlen(*pcFile) == 0)
               fprintf(fLogFile, "   disk device %s\n", *pcTape);
            else
               fprintf(fLogFile,
                  "    file %s on disk %s\n", *pcFile, *pcTape);
         }
         break;
      case 2:
         if (iDebug)
         {
            if (strlen(*pcFile) == 0)
               fprintf(fLogFile, "   tape device %s\n", *pcTape);
            else
               fprintf(fLogFile, "    file %s on tape %s\n", *pcFile, *pcTape);
         }
         break;
      default:
         fprintf(fLogFile, "-E- invalid file name %s\n", pcFile0);
         return(-1);

   } /* switch(iDevType) */
#endif /* VMS */

   if (iDevType == 2)                                       /* tape */
   {
#ifndef VMS
      /* check string after 1st device delimiter */
      iRC = strncmp(pcc, pcDevDelim, 1);
      if (iRC == 0)
      {
         fprintf(fLogFile,
            "-E- node specification not allowed in file name: %s\n",
            pcFile0);
         return(-2);
      }
#endif /* Unix */

      strncpy(plocd++, "\0", 1);              /* cut string at colon */
      strcpy(pBufTape, cTemp);
#ifdef VMS
      strncat(pBufTape, pcDevDelim, 1);       /* append colon in VMS */

#else
      pgen = strchr(pBufTape, *pcStar);
      if (pgen != NULL)
      {
         fprintf(fLogFile,
            "-E- specified device %s has generic path\n", pBufTape);
         fprintf(fLogFile,
            "    only the relative file name may be generic\n");
         return(-1);
      }
#endif

      strcpy(pBufFile, plocd);
      if (iDebug) fprintf(fLogFile,
         "    file %s on tape %s\n", pBufFile, pBufTape);
      strcpy(cText, "tape");
      iRC = 1;

   } /* (iDevType == 2) */
   else
   {
      strcpy(pBufFile, cTemp);
      strcpy(cText, "disk");
      iRC = 0;
   }

   if (iDebug) fprintf(fLogFile,
      "    %s file %s\n", cText, pBufFile);

   pdelim = strrchr(pBufFile, *pcFileDelim);
   if (pdelim != NULL)                             /* path specified */
   {
#ifndef VMS
      if (iDevType == 2)                             /* file on tape */
      {
         fprintf(fLogFile,
            "-E- path in tape file name not allowed: %s\n", pBufFile);
         return(-1);
      }
#endif

      /* disk file prefixed with path */
      pgen = strchr(pBufFile, *pcStar);
      if (pgen == NULL)
         pgen = strchr(pBufFile, *pcQM);
      if ( (pgen != NULL) && (pgen < pdelim) )
      {
         fprintf(fLogFile,
            "-E- specified %s file %s has wildcard chars in path name\n",
            cText, pBufFile);
         fprintf(fLogFile,
            "    only the relative file name may contain wildcard chars\n");
         return(-1);
      }

      pdelim++;                          /* points to rel. file name */

   } /* (pdelim != NULL) */
   else pdelim = pBufFile;

   ii = MAX_OBJ_LL - 2;
   if (strlen(pdelim) > ii)
   {
      fprintf(fLogFile,
         "-E- rel file name %s too long:\n    has %d chars, max allowed %d\n",
         pdelim, strlen(pdelim), ii);
      return(-1);
   }

#ifndef VMS
   /* tape: convert file name from uppercase if necessary */
   if (iDevType == 2)
   {
      pcc = pdelim;
      while (*pcc != '\0')
      {
         iloc = isupper(*pcc);
         if (iloc != 0)
         {
            if (!bup) fprintf(fLogFile,
               "-W- upper case in (relative) file name not supported: %s\n",
               pdelim);
            bup = bTrue;
            *pcc = tolower(*pcc);            /* change to lower case */
         }
         pcc++;
      } /* while (*pcc != '\0') */
   }

   if (bup)
      fprintf(fLogFile, "    instead assumed: %s\n", pdelim);
#endif

   if (iDebug) fprintf(fLogFile,
      "\n    end %s: file name %s okay\n", cModule, pcFile0);

   return(iRC);

} /* rawCheckClientFile */

/**********************************************************************
 * rawGetFSpName
 *    get file space name from user specification
 * created 22.3.1996, Horst Goeringer
 **********************************************************************
 */

char *rawGetFSpName( char *pcUser )
{
   char cModule[32]="rawGetFSpName";
   int iDebug = 0;

   char *pc;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   pc = pcUser;
   if ( (strchr(pcUser, *pcStar) != NULL) ||
        (strchr(pcUser, *pcQM) != NULL) )
   {
      fprintf(fLogFile,
         "-E- %s: generic archive name '%s' not allowed\n",
         cModule, pcUser);
      return(pcNull);
   }

   strcpy(cNamefs, "");                    /* initialize */
   if (iDebug)
      fprintf(fLogFile, "-D- %s: FS %s, in %s-%s, delim %s\n",
              cModule, cNamefs, pcUser, pc, pcObjDelim);
   if (strncmp(pc, pcObjDelim, 1) == 0)
      pc++;
   else
      /* strncpy(cNamefs, pcObjDelim, 1); */
      /* in adsmcli session: beginning with 2nd invocation,
         gives incorrect results due to missing \0 ! H.G. 27.10.97 */ 
      strcpy(cNamefs, pcObjDelim);

   if (isalpha(*pc) == 0)  
   {
      fprintf(fLogFile,
              "-E- archive name '%s' must start with a letter\n", pc);
      return(pcNull);
   }

   strcat(cNamefs, pcUser); 
   if (iDebug) fprintf(fLogFile,
      "-D- end %s: FS %s\n", cNamefs, cModule);

   return( (char *) cNamefs);

} /* rawGetFSpName */

/*********************************************************************
 * rawGetHLName: get high level object name from path
 *
 * insert name part at beginning depending on device
 * PLATFORM DEPENDENT
 *
 * created 16. 2.96, Horst Goeringer
 *********************************************************************
 */

char *rawGetHLName( char *pcPath) 
{
   char cModule[32]="rawGetHLName";
   int iDebug = 0;

   char *pcc;
   char *pdelim;
   char cNameTemp[MAX_OBJ_HL] = "";
   int ii = 0, ii1 = 0;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s\n    path specified: %s\n", cModule, pcPath);

   pdelim = strchr(pcPath, *pcObjDelim);
   if (pdelim != pcPath)     
   {
      strcpy(cNamehl, pcObjDelim);
      if (iDebug) fprintf(fLogFile,
         "    delimiter '%s' inserted at begin of archive path\n",
         pcObjDelim); 
      strcat(cNamehl, pcPath);
   }
   else strcpy(cNamehl, pcPath);

   pdelim = strstr(cNamehl, "/./");
   if (pdelim != NULL)
   {
      strcpy(cNameTemp, cNamehl);
      pdelim = strstr(cNameTemp, "/./");

      while (pdelim != NULL)
      {
         ii++;
         strncpy(pdelim, "\0", 1);
         strcpy(cNamehl, cNameTemp);
         pdelim++;
         pdelim++;
         strcat(cNamehl, pdelim);

         strcpy(cNameTemp, cNamehl);
         pdelim = strstr(cNameTemp, "/./");
      }
      if (iDebug) fprintf(fLogFile,
         "    %d unnecessary './' removed\n", ii);
   }

   pdelim = strstr(cNamehl, "/.");
   if (pdelim != NULL)
   {
      pcc= cNamehl;
      ii = strlen(cNamehl);
      ii1 = pdelim - pcc;
      if (ii1 < 0)
         ii1 = -ii1;
      if (ii1+2 == ii)
      {
         strncpy(pdelim, "\0", 1);
         if (iDebug) fprintf(fLogFile,
            "    trailing '/.' removed\n");
      }
   }

   if (iDebug) fprintf(fLogFile,
      "    hl name: %s\n-D- end %s\n\n", cNamehl, cModule);

   return(cNamehl);

} /* rawGetHLName */
 
/*********************************************************************
 * rawGetLLName: get low level object name from file name
 *    PLATFORM DEPENDENT
 *
 * created 16. 2.1996, Horst Goeringer
 *********************************************************************
 *  9. 1.2002, H.G.: pass object and delimiter as argument
 *********************************************************************
 */

int rawGetLLName( char *pcFile, const char *pcDelimiter,
                  char *pcObject)
{
   char cModule[32] = "rawGetLLName";
   int iDebug = 0;

   char cNamell[MAX_OBJ_LL] = "";
   char *ploc = NULL;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s\n    file %s, object delimiter %s\n",
      cModule, pcFile, pcDelimiter);

   strcpy(cNamell, pcDelimiter);

   ploc = strrchr(pcFile, *pcFileDelim);
   if (ploc != NULL)
      strcat(cNamell, ++ploc);   /* begin copy after file delimiter */
   else 
   {
      ploc = strrchr(pcFile, *pcObjDelimAlt);
      if (ploc != NULL)
         strcat(cNamell, ++ploc);/* begin copy after file delimiter */
      else 
      {
#ifdef VMS
         ploc = strrchr(pcFile, *pcDevDelim); /* look for disk device */
         if (ploc != NULL)
            strcat(cNamell, ++ploc);
                                /* begin copy after device delimiter */
         else
#endif
         strcat(cNamell, pcFile);
      }
   }
   strcpy(pcObject, cNamell);

   if (iDebug) fprintf(fLogFile,
      "    ll name %s\n-D- end %s\n\n", pcObject, cModule);

   return 0;

} /* rawGetLLName */

/**********************************************************************
 * rawGetFileSize
 *    get file size (bytes)
 * created 19.2.2002, Horst Goeringer
 **********************************************************************
 */

int rawGetFileSize(char *pcFile,
                   unsigned long *piSize,
                   unsigned int *piRecl)      /* needed only for VMS */
{
   char cModule[32] = "rawGetFileSize";
   int iDebug = 0;

   int iRC;
   int iRecl = 0;
   unsigned long iSize = 0;

#ifdef WIN32          /* Windows */
   struct _stat sFileStatus, *pFileStatus;
#else                 /* Unix or VMS */
   struct stat sFileStatus, *pFileStatus;
#endif

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: file %s\n", cModule, pcFile);

   pFileStatus = &sFileStatus;

#ifdef WIN32          /* Windows */
   iRC = _stat(pcFile, pFileStatus);
#else                 /* Unix or VMS */
   iRC = stat(pcFile, pFileStatus);
#endif
   if (iRC)
   {
      fprintf(fLogFile,
         "-E- %s: file %s unavailable (stat)\n", cModule, pcFile);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));

         /* valid only for 32 bit OS */
         if (strcmp(strerror(errno),
               "Value too large for defined data type") == 0)
         {
            fprintf(fLogFile,
               "-E- %s: file size of %s > 2 GByte: use 64 bit gStore client\n",
               cModule, pcFile);

            errno = 0;
            return -99;
         }
         errno = 0;
      }

      return -1;
   }

#ifdef VMS
      iRecl = pFileStatus->st_fab_mrs;
#else
#ifdef WIN32
      iRecl = 0;    /* not available in Windows */
#else
      iRecl = pFileStatus->st_blksize;
#endif
#endif
   iSize = pFileStatus->st_size;
   if (pFileStatus->st_mode & S_IFREG)
   {
      iRC = 0;
      if (iDebug) fprintf(fLogFile,
         "    file %s: size %u, recl %d (byte)\n",
         pcFile, iSize, iRecl);
   }
   else
   {
      if (pFileStatus->st_mode & S_IFDIR)
      {
         iRC = 1;
         if (iDebug) fprintf(fLogFile,
            "-W- %s is a directory, size %u\n", pcFile, iSize);
      }
#ifndef WIN32
#ifndef VMS
      else if (pFileStatus->st_mode & S_IFLNK)
      {
         iRC = 2;
         if (iDebug) fprintf(fLogFile,
            "-W- %s is a symbolic link, size %u\n", pcFile, iSize);
      }
#endif
#endif
      else
      {
         iRC = 3;
         if (iDebug) fprintf(fLogFile,
            "-W- unexpected item %s, size %u\n", pcFile, iSize);
      }
   }

   *piRecl = iRecl;
   *piSize = iSize;
   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iRC;

} /* rawGetFileSize */

/*********************************************************************
 * rawGetPathName:   get path name from high level object name
 *
 * created 21. 3.1996, Horst Goeringer
 *********************************************************************
 * 23. 4.1996, H.G: no more device dependency in hl-name, as with
 *                  ADSM V213 management class can be set in dsmSendObj
 *********************************************************************
 */

char *rawGetPathName( char *pcNamehl) 
{
   char cModule[32] = "rawGetPathName";
   int iDebug = 0;

   char *ploc = NULL, *ploc1 = NULL;
   char *pcc;

   if (iDebug)
      fprintf(fLogFile, "-D- begin %s: hl: %s\n", cModule, pcNamehl);

   if (strlen(pcNamehl) == 0)
   {
      fprintf(fLogFile, "-E- %s: high level name empty\n", cModule);
      return(pcNull);
   }

   pcc = pcNamehl;
   ploc = strchr(pcNamehl, *pcObjDelim);
   ploc1 = strchr(pcNamehl, *pcObjDelimAlt);
   if ( (ploc != pcc) && (ploc1 != pcc) )
   {
      fprintf(fLogFile, 
		      "-E- %s: invalid prefix in high level name %s\n",
              cModule, pcNamehl);
      return(pcNull);
   }

   strcpy(cPath, pcc);
   if (iDebug) fprintf(fLogFile, 
		"-D- end %s: path %s\n\n", cModule, cPath);

   return cPath; 

} /* rawGetPathName */

/********************************************************************
 * rawQueryFile: get query information for single file
 *
 * used for tape input handled sequentially
 * buffers for communication and query info allocated in calling
 *    procedure
 ********************************************************************
 */

int rawQueryFile(
       int iSocket,
       int iAltMaster,                          /* currently unused */
       srawComm *pCommBuf,
       void **pQueryBuf) 
{
   char cModule[32] = "rawQueryFile";
   int iDebug = 0;

   int iATLServer = -1;         /* =0: all, =1: aixtsm1, =2: gsitsma */
   int iNameWC = -1;
   int iRC;
   int iAction;
   int iIdent, iQuery = -1, iAttrLen;
   int iQueryAll = 0;
   int iStatus, iStatusLen;
   int iBuf, iBufComm;
   int ii, jj;
   char *pcc;

   int iVersionObjAttr = 0;
      /* version no. of srawObjAttr:
         =3:  288 byte cache buffer, 2 restore fields
         =4:  304 byte, 5 restore fields, ATL server no.
         =5:  384 byte, maior enhancement */
   int iPoolId = 0;
   int iObjLow = 0;
   int iFS = 0;

   srawComm *pComm; 
   srawQueryResult *pQuery;
   srawObjAttr *pObjAttr;

   pComm = pCommBuf;
   iAction = ntohl(pComm->iAction);
   iATLServer = ntohl(pComm->iATLServer);
   pQuery = (srawQueryResult *) (*pQueryBuf);

   iBufComm = ntohl(pComm->iCommLen) + HEAD_LEN;
   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: query file %s in ATL Server %d (action %d)\n",
      cModule, pComm->cNamell, iATLServer, iAction);

   switch(iAction)
   {
      case QUERY:
      case QUERY_ARCHIVE:
      case QUERY_ARCHIVE_RECORD:
      case QUERY_ARCHIVE_TO_CACHE:
      case QUERY_ARCHIVE_MGR:
      case QUERY_REMOVE:
      case QUERY_REMOVE_MGR:
      case QUERY_RETRIEVE:
      case QUERY_RETRIEVE_RECORD:
      case QUERY_STAGE:
         ;                          /* okay */
         break;
      default:
         fprintf(fLogFile,
            "-E- %s: invalid action %d\n", cModule, iAction);
         iQuery = -1;
         goto gEndQueryFile;
   }

   pcc = (char *) pComm;
   iRC = send(iSocket, pcc, (unsigned) iBufComm, 0);
   if (iRC < iBufComm)
   {
      if (iRC < 0) fprintf(fLogFile,
         "-E- %s: sending command buffer for query file %s\n",
         cModule, pComm->cNamell);
      else fprintf(fLogFile,
         "-E- %s: query command buffer incompletely sent (%d of %d byte)\n",
         cModule, iRC, iBufComm);

      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

      iQuery = -1;
      goto gEndQueryFile;
   }

   if (iDebug) fprintf(fLogFile,
      "    query command sent to entry server (%d bytes), look for reply\n",
      iBufComm);

   /******************* look for reply from server *******************/

gNextReply:
   pcc = (char *) pQuery;
   iRC = rawRecvHead(iSocket, pcc);
   if (iRC <= 0)
   {
      fprintf(fLogFile,
         "-E- %s: receiving buffer header with query information\n",
         cModule);
      iQuery = -1;
      goto gEndQueryFile;
   }

   pcc += HEAD_LEN;  
   iIdent = ntohl(pQuery->iIdent);
   iQuery = ntohl(pQuery->iObjCount);
   iAttrLen = ntohl(pQuery->iAttrLen);

   if (iDebug) fprintf(fLogFile,
      "    %d bytes received: iIdent %d, iQuery %d, iAttrLen %d\n",
      iRC, iIdent, iQuery, iAttrLen);

   if ( (iIdent != IDENT_QUERY) && (iIdent != IDENT_QUERY_DONE) &&
        (iIdent != IDENT_QUERY_ARCHDB) )
   {
      iStatus = iQuery;
      iQuery = -1;                                   /* query failed */

      if (iIdent == IDENT_STATUS)
      {
         iStatusLen = iAttrLen;
         if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) ||
              (iStatus == STA_ARCHIVE_NOT_AVAIL) ||
              (iStatus == STA_NO_ACCESS) )
         {
            if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) )
               fprintf(fLogFile,
                  "-E- %s: error status from entry server instead of query data:\n",
                  cModule);

            if (iStatusLen > 0)
            {
               iRC = rawRecvError(iSocket, iStatusLen, pcc);
               if (iRC < 0) fprintf(fLogFile,
                  "-E- receiving error msg from server, rc = %d\n",
                  iRC);
               else if ( (iDebug) || (iStatus == STA_ERROR) ||
                         (iStatus == STA_ERROR_EOF) )
                  fprintf(fLogFile, "%s", pcc);
            }
            else
               fprintf(fLogFile,"    no error message available\n");

            if (iStatus == STA_ARCHIVE_NOT_AVAIL)
               iQuery = -1000;
            else if (iStatus == STA_NO_ACCESS)
               iQuery = -1001;

         } /* (iStatus == STA_ERROR || STA_ERROR_EOF ||
               STA_ARCHIVE_NOT_AVAIL || STA_NO_ACCESS) */
         else fprintf(fLogFile,
           "-E- %s: unexpected status (type %d) received from server\n",
           cModule, iStatus);

      } /* iIdent == IDENT_STATUS */
      else fprintf(fLogFile,
         "-E- %s: unexpected header (type %d) received from server\n",
         cModule, iIdent);

      iQueryAll = iQuery;
      goto gEndQueryFile;

   } /* iIdent != IDENT_QUERY  && != IDENT_QUERY_DONE &&
               != IDENT_QUERY_ARCHDB */

   iQueryAll += iQuery;
   if (iQuery > 0)
   {
      pcc = (char *) pQuery + HEAD_LEN;
      pObjAttr = (srawObjAttr *) pcc;

      /* DDD iQuery = 1 only! */
      for (ii=1; ii<=iQuery; ii++)
      {
         iBuf = iAttrLen;        /* only space for one object buffer */
         while(iBuf > 0)
         {
            if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
            {
               if (iRC < 0)
               {
                  fprintf(fLogFile,
                     "-E- %s: receiving buffer %d with query results\n",
                     cModule, ii);

                  iQuery = -1;
                  iQueryAll -= iQuery;
               }
               else
               {
                  jj = iAttrLen - iBuf;
                  fprintf(fLogFile,
                     "-E- %s: connection to server broken, only %d bytes of query object %d (%d byte) received\n",
                     cModule, jj, ii, iAttrLen);

                  if (ii == 1)
                  {
                     iQuery = -1;
                     iQueryAll -= iQuery;
                  }
                  else
                  {
                     fprintf(fLogFile,
                        "-E- %s: only %d of %d objects received\n",
                        cModule, --ii, iQuery);
                     iQueryAll -= iQuery - ii;
                     iQuery = ii;
                  }
               }
            
               if (errno)
               {
                  fprintf(fLogFile, "    %s\n", strerror(errno));
                  errno = 0;
               }

               goto gEndQueryFile;
            }

            iBuf -= iRC;
            pcc += iRC;

         } /* while(iBuf > 0) */

         iVersionObjAttr = ntohl(pObjAttr->iVersion);
         if ( (iVersionObjAttr != VERSION_SRAWOBJATTR) &&
              (iVersionObjAttr != VERSION_SRAWOBJATTR-1) )
         {
            fprintf(fLogFile,
               "-E- %s: invalid cacheDB entry version %d\n",
               cModule, iVersionObjAttr);

            iQuery = -1;
            goto gEndQueryFile;
         }

         iPoolId = ntohl(pObjAttr->iPoolId);
         if (iDebug) 
         {
            if (ii == 1) fprintf(fLogFile,
               "    query buffer received (%d bytes, objAttr data V%d)\n",
               iAttrLen, iVersionObjAttr);
            else fprintf(fLogFile,
               "    query buffer overlaid (%d bytes)\n", iAttrLen);

            fprintf(fLogFile, "    %s%s%s: poolId %d",
               pObjAttr->cNamefs, pObjAttr->cNamehl,
               pObjAttr->cNamell, iPoolId); 
            if (iPoolId == 1)
               fprintf(fLogFile, " (RetrievePool)\n");
            else if (iPoolId == 2)
               fprintf(fLogFile, " (StagePool)\n");
            else if (iPoolId == 3)
               fprintf(fLogFile, " (ArchivePool)\n");
            else if (iPoolId == 4)
               fprintf(fLogFile, " (DAQPool)\n");
            else
               fprintf(fLogFile, " (TSM DB)\n");
         }
      } /* object loop */
   } /* (iQuery > 0) */
   else if ( (iQuery == 0) && (iDebug) )
     fprintf(fLogFile,
        "    no more objects found in ATL server %d\n", iATLServer);

   /* if object explicitly specified, stop query if found */
   if ( (strchr(pComm->cNamell, *pcStar) == NULL) &&
        (strchr(pComm->cNamell, *pcQM) == NULL) &&
        (strchr(pComm->cNamell, *pcPerc) == NULL) &&
        (strchr(pComm->cNamehl, *pcStar) == NULL) &&
        (strchr(pComm->cNamehl, *pcQM) == NULL) &&
        (strchr(pComm->cNamehl, *pcPerc) == NULL) )
      iNameWC = 0;
   else
      iNameWC = 1;

   if ( (iNameWC == 0) && (iQuery > 0) )      /* fixed name, found */
      goto gEndQueryFile;

   if ( (iIdent == IDENT_QUERY) || (iIdent == IDENT_QUERY_ARCHDB) )
      goto gNextReply;
 
gEndQueryFile:
   switch (iQueryAll)
   {
      case 0:
         if (iDebug) fprintf(fLogFile,
            "    no matching object %s in gStore found\n",
            pComm->cNamell);
         break;
      case 1:
         if (iDebug) fprintf(fLogFile,
            "    file %s available in gStore\n", pComm->cNamell);
         break;
      case -1:
         fprintf(fLogFile,
            "-E- %s: query in gStore could not be executed\n",
            cModule);
         iQuery = -1;
         break;
      case -1000:
         if (iDebug) fprintf(fLogFile,
            "    requested archive %s not existing in gStore\n",
            pComm->cNamefs);
         break;
      case -1001:
         if (iDebug) fprintf(fLogFile,
            "    requested access to file %s not allowed\n",
            pComm->cNamell);
         break;
      default:
         if (iQueryAll > 1)
         {
            if (strcmp(pComm->cOwner, "goeri") == 0)
               fprintf(fLogFile, "-W- %d versions of %s exist!\n", 
                  iQuery, pComm->cNamell);
         }
         else
         {
            fprintf(fLogFile,
               "-E- %s: invalid number %d of objects %s found\n",
               cModule, iQuery, pComm->cNamell);
            iQuery = -1;
         }
   } /* switch (iQuery) */

   if (iDebug) fprintf(fLogFile,
      "-D- end %s\n\n", cModule);

   return iQuery;

} /* end rawQueryFile */

/*********************************************************************/
/* rawRecvError:   receive error message */
/*    returns length of error message */
/* */
/* created 14. 3.96, Horst Goeringer */
/*********************************************************************/

int rawRecvError(int iSocket, int iLen, char *pcMsg)
{
   char cModule[32]="rawRecvError";
   int iDebug = 0;

   int iRC = 0, ii;
   int iBuf, iBufs;
   char *pcc;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   strcpy(pcMsg, "");
   pcc = pcMsg;         /* points now to buffer in calling procedure */

   iBuf = iLen;                           /* length of error message */
   iBufs = iBuf;
   while(iBuf > 0)
   {
      if ( (iRC = recv(iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
      {
         if (iRC < 0)
         {
            fprintf(fLogFile,
               "-E- %s: receiving error message\n", cModule);

            if (errno)
            {
               fprintf(fLogFile, "    %s\n", strerror(errno));
               errno = 0;
            }

            iRC = -9;
            break;
         }
         else
         {
            ii = iLen - iBuf;
            if (ii) /* append delimiters after part of message received */
            {
               *pcc = '\0';                   /* delimit message string */
               pcc++;
               *pcc = '\n';
               pcc++;
               if (iDebug) fprintf(fLogFile, 
                  "-E- incomplete error message received:\n    %s", pcMsg);
            }
            fprintf(fLogFile,
               "-E- %s: connection to sender broken, %d byte of error message (%d byte) received\n",
               cModule, ii, iLen);

            if (errno)
            {
               fprintf(fLogFile, "    %s\n", strerror(errno));
               errno = 0;
            }

            iRC = -8;
            break;
         }
      }

      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   if (iBuf < 0)
   {
      fprintf(fLogFile,
         "-E- %s: more error data received than expected:\n     %s",
         pcMsg, cModule);
      iRC = -2;
   }

   if (iRC == -9)
   {
      if (iDebug)
         fprintf(fLogFile, "-D- end %s\n\n", cModule);

      return iRC;
   }
   else
   {
      /* also okay if no message (iLen = 0) */
      *pcc = '\0';                         /* delimit message string */

      /* only msg part received */
      if (iRC == -8)
         iBufs = ii;

      if (iDebug)
      {
         fprintf(fLogFile,
            "    error message received (%d bytes):\n    %s",
            iBufs, pcMsg);
         fprintf(fLogFile, "-D- end %s\n\n", cModule);
      }

      return iBufs;
   }

} /* rawRecvError */

/*********************************************************************
 * rawRecvHead: receive common buffer header
 *    returns no. of bytes received or error (< 0)
 *
 * created  5. 3.96, Horst Goeringer
 *********************************************************************
 */

int rawRecvHead( int iSocket, char *pcBuf)
{
   char cModule[32]="rawRecvHead";
   int iDebug = 0;

   int iBuf, iBufs;
   int iRC, ii;
   int iIdent;
   int iStatus;
   int iDataLen;
   char *pcc;
   int *pint;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   strcpy(pcBuf, "");
   pcc = pcBuf;        /* points now to buffer in calling procedure */
   pint = (int *) pcc;

   iBuf = HEAD_LEN;
   iBufs = iBuf;
   while(iBuf > 0)
   {
      iRC = recv(iSocket, pcc, (unsigned) iBuf, 0);
      if (iRC <= 0)
      {
         if (iRC < 0) fprintf(fLogFile,
            "-E- %s: receiving buffer header\n", cModule);
         else
         {
            ii = iBufs - iBuf;
            fprintf(fLogFile,
               "-E- %s: connection to sender broken, %d byte of buffer header (%d byte) received\n",
               cModule, ii, iBufs);
         }

         if (errno)
         {
            fprintf(fLogFile, "    %s\n", strerror(errno));
            errno = 0;
         }

         return -1;
      }

      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   iIdent = ntohl(*pint);
   pint++;
   iStatus = ntohl(*pint);
   pint++;
   iDataLen = ntohl(*pint);
   if (iDebug) fprintf(fLogFile,
      "    ident %d, status %d, datalen %d\n",
      iIdent, iStatus, iDataLen);

   if ( (iIdent == IDENT_STATUS) &&
        ((iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF)) &&
        (iDataLen > 0) )
   {
      if (iDebug) fprintf(fLogFile,
         "-W- %s: error message available for receive (%d byte)\n",
         cModule, iDataLen);
      return iDataLen;
   }

   if (iDebug) fprintf(fLogFile,
      "-D- end %s: buffer header received (%d bytes)\n\n",
      cModule, iBufs);

   return iBufs;

} /* rawRecvHead */

/*********************************************************************
 * rawRecvHeadC: receive common buffer header and check
 *    return values:
 *           >= 0: no error occurred:
 *     = HEAD_LEN: header received (and okay)
 *     > HEAD_LEN: status message of this size in pcMsg
 *           =  0: error status received, but no message
 *
 *           <  0: error occurred:
 *           = -1: recv failed
 *           = -2: connection to sender lost
 *           = -3: receiving error msg failed
 *           = -4: unexpected identifier received
 *           = -5: unexpected status received
 *
 * created  7. 3.2002, Horst Goeringer
 *********************************************************************
 */

int rawRecvHeadC(int iSocket,
                 char *pcBuf,
                 int iIdentReq,     /* < 0 => check, = 0 => no check */
                 int iStatusReq,   /* >= 0 => check, < 0 => no check */
                 char *pcMsg)
{
   char cModule[32] = "rawRecvHeadC";
   int iDebug = 0;

   /* header to be received */
   int iIdent;
   int iStatus;
   int iDataLen;

   char cMsg1[STATUS_LEN] = "";

   int iRC, ii;
   int iBuf, iBufs;
   char *pcc;
   int *pint;

   if (iDebug)
   {
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

      if (iIdentReq < 0) fprintf(fLogFile,
         "    check Ident, expect %d\n", iIdentReq);
      else
         fprintf(fLogFile, "    no check of Ident\n");
      if (iStatusReq >= 0) fprintf(fLogFile,
         "    check Status, expect %d\n", iStatusReq);
      else
         fprintf(fLogFile, "    no check of Status\n");
   }

   strcpy(pcMsg, "");
   strcpy(pcBuf, "");
   pcc = pcBuf;         /* points now to buffer in calling procedure */

   iBuf = HEAD_LEN;
   iBufs = iBuf;
   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
      {
         if (iRC < 0)
         {
            sprintf(pcMsg, "-E- %s: receiving buffer header\n",
               cModule);
            iRC = -1;
         }
         else
         {
            ii = iBufs - iBuf;
            sprintf(pcMsg,
               "-E- %s: connection to sender broken, %d byte of buffer header (%d byte) received\n",
               cModule, ii, iBufs);
            iRC = -2;
         }

         if (errno)
         {
            sprintf(cMsg1, "    %s\n", strerror(errno));
            strcat(pcMsg, cMsg1);
            errno = 0;
         }

         if (iDebug)
            fprintf(fLogFile, pcMsg);

         goto gEndRecvHeadC;
      }

      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   if (iDebug) fprintf(fLogFile,
      "    buffer header received (%d bytes)\n", iBufs);

   pint = (int *) pcBuf;
   iIdent = ntohl(*pint);
   pint++;
   iStatus = ntohl(*pint);
   pint++;
   iDataLen = ntohl(*pint);

   if (iDebug) fprintf(fLogFile,
      "    ident %d, status %d, datalen %d\n",
      iIdent, iStatus, iDataLen);

   if (iIdent == IDENT_STATUS)
   {
      if (iDebug)
      {
         fprintf(fLogFile, "    status received");
         if ( (iDataLen) &&
              ((iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF)) )
            fprintf(fLogFile, " with error message\n");
         else
            fprintf(fLogFile, "\n");
      }

      if ( (iDataLen) &&
           ((iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) ||
            (iStatus == STA_CACHE_FULL)) )
      {
         pcc = cMsg1;
         iRC = rawRecvError(iSocket, iDataLen, pcc);
         if (iRC < 0)
         {
            sprintf(pcMsg, "-E- %s: receiving error msg, rc=%d\n",
               cModule, iRC);
            if (iDebug)
               fprintf(fLogFile, pcMsg);

            iRC = -3;
            goto gEndRecvHeadC;
         }

         if (iDebug) fprintf(fLogFile,
            "    msg (%d byte): %s\n", iDataLen, pcc);
         strcat(pcMsg, pcc);
         iRC = strlen(pcMsg);
         goto gEndRecvHeadC;
      }

      if ( (iDataLen == 0) &&
           ((iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF)) )
      {
         strcat(pcMsg,
            "-W- error status received, but no error message\n");
         iRC = 0;
         goto gEndRecvHeadC;
      }
   } /* (iIdent == IDENT_STATUS) */

   if (iIdentReq < 0)    /* test for expected identifier, values < 0 */
   {
      if (iDebug)
         fprintf(fLogFile, "    check identifier\n");

      if (iIdent != iIdentReq)
      {
         sprintf(pcMsg,
            "-E- %s: unexpected header (ident %d) received\n",
            cModule, iIdent);
         if (iDebug)
            fprintf(fLogFile, pcMsg);

         iRC = -4;
         goto gEndRecvHeadC;

      } /* unexpected ident) */
      else
      {
         if (iStatusReq >= 0)            /* test for expected status */
         {
            if (iDebug)
               fprintf(fLogFile, "    check status\n");

            if (iStatusReq != iStatus)
            {
               sprintf(pcMsg,
                  "-E- %s: unexpected header (status %d) received\n",
                  cModule, iStatus);
               if (iDebug)
                  fprintf(fLogFile, pcMsg);

               iRC = -5;
               goto gEndRecvHeadC;
            }
         }
      } /* expected ident */
   } /* check ident requested */

   iRC = HEAD_LEN;

gEndRecvHeadC:
   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iRC;

} /* rawRecvHeadC */

/*********************************************************************
 * rawRecvRequest:
 *    receive request for next buffer and convert to host format
 *    returns   0: request received
 *              2: EOF status received
 *              3: EOS status received
 *           4, 5: error status received
 *            < 0: error occured
 *
 * created 21.12.2000, Horst Goeringer
 *********************************************************************
 */

int rawRecvRequest(int iSocket,
                   int *piSeekMode, 
                   int *piOffset,
                   int *piBufferSize)
{
   char cModule[32]="rawRecvRequest";
   int iDebug = 0;

   int iRC;
   int iError = 0;
   int iRequSize = sizeof(srawRequest);
   int iBuf, iBuf0;
   int ii, iimax;
   char *pcc;
   char cMsg[STATUS_LEN] = "";                 /* error msg received */

   srawRequest sRequest, *pRequest;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: receive request buffer\n", cModule);

   pRequest = &sRequest;
   pcc = (char *) pRequest;
   iBuf = HEAD_LEN;
   iimax = HEAD_LEN;
   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
      {
         if (iRC < 0)
         {
            fprintf(fLogFile,
               "-E- %s: receiving buffer header\n", cModule);
            iError = -1;
         }
         else
         {
            ii = iimax - iBuf;
            fprintf(fLogFile,
               "-W- %s: connection to sender broken, %d byte of buffer header (%d byte) received\n",
               cModule, ii, iimax);
            iError = -5;
         }

         goto gErrorRecvRequest;
      }
      
      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   if (iBuf < 0) 
   {
      fprintf(fLogFile,
         "-E- %s: more buffer header data received than expected\n",
         cModule);

      iError = -2;
      goto gErrorRecvRequest;
   }

   pRequest->iIdent = ntohl(pRequest->iIdent);
   if (iDebug) fprintf(fLogFile,
      "    buffer header received (%d bytes, id %d)\n",
      iimax, pRequest->iIdent);

   if ( (pRequest->iIdent != IDENT_NEXT_BUFFER) &&
        (pRequest->iIdent != IDENT_STATUS) )
   {
      fprintf(fLogFile, "-E- %s: invalid buffer received (id %d)\n", 
              cModule, pRequest->iIdent);
      iError = -3;
      goto gErrorRecvRequest;
   }

   pRequest->iStatus = ntohl(pRequest->iStatus);
   pRequest->iStatusLen = ntohl(pRequest->iStatusLen);
   iBuf0 = pRequest->iStatusLen;
   iBuf = iBuf0;
   if (pRequest->iIdent != IDENT_NEXT_BUFFER)
      pcc = cMsg;                               /* more space needed */

   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
      {
         if (iRC < 0)
         {
            fprintf(fLogFile,
               "-E- %s: receiving buffer data\n", cModule);
            iError = -1;
         }
         else
         {
            ii = iBuf0 - iBuf;
            fprintf(fLogFile,
               "-W- %s: connection to sender broken, %d byte of data (%d byte) received\n",
               cModule, ii, iBuf0);
            iError = -5;
         }

         if (errno)
         {
            fprintf(fLogFile, "    %s\n", strerror(errno));
            errno = 0;
         }

         goto gErrorRecvRequest;
      }
      
      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   if (iBuf < 0) 
   {
      fprintf(fLogFile, "-E- %s: more data received than expected\n",
         cModule);
      iError = -2;
      goto gErrorRecvRequest;
   }

   if (pRequest->iIdent == IDENT_NEXT_BUFFER)
   {
      if (iDebug) fprintf(fLogFile,
         "    %s: request data received (%d bytes)\n", cModule, iBuf0);

      if (iBuf0 + HEAD_LEN != iRequSize)
      {
         fprintf(fLogFile,
            "-E- %s: invalid data size (%d) in request buffer (expected %d byte)\n",
            cModule, iBuf0, iRequSize-HEAD_LEN);
         iError = -4;
         goto gErrorRecvRequest;
      }

      *piSeekMode = ntohl(pRequest->iSeekMode);
      *piOffset = ntohl(pRequest->iOffset);
      *piBufferSize = ntohl(pRequest->iBufferSize);

      if (iDebug)
         fprintf(fLogFile, "-D- end %s\n\n", cModule);

      return 0;

   } /* (pRequest->iIdent == IDENT_NEXT_BUFFER) */
   else if (pRequest->iIdent == IDENT_STATUS)
   {
      if ( (pRequest->iStatus == STA_END_OF_FILE) ||
           (pRequest->iStatus == STA_END_OF_SESSION) )
      {
         if (iDebug) fprintf(fLogFile,
            "    %s: status info received: end session\n", cModule);
      }
      else if ( (pRequest->iStatus == STA_ERROR) ||
                (pRequest->iStatus == STA_ERROR_EOF) )

      {
         if (iDebug) fprintf(fLogFile,
            "-W- %s: error status received: end session\n", cModule);
         if (iBuf0 > 0) fprintf(fLogFile,
            "-W- %s: error message from client:\n    %s\n",
            cModule, cMsg);
         iError = -4;
         goto gErrorRecvRequest;
      }
      else
      {
         fprintf(fLogFile,
            "-E- %s: invalid status buffer received (id %d)\n",
            cModule, pRequest->iStatus);
         iError = -3;
         goto gErrorRecvRequest;
      }

      iError = pRequest->iStatus;
      goto gErrorRecvRequest;

   } /* (pRequest->iIdent == IDENT_STATUS) */
   else
   {
      fprintf(fLogFile, "-E- %s: invalid buffer received (ident %d)\n",
              cModule, pRequest->iIdent);
      iError = -2;
      goto gErrorRecvRequest;
   }

gErrorRecvRequest:
   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   if (iError)
      return iError;

   return 0;

} /* rawRecvRequest */

/*********************************************************************
 * rawRecvStatus: receive status header
 *    returns no. of bytes received or error (< 0)
 *    status header will be converted to host format
 *
 * created 18. 3.96, Horst Goeringer
 *********************************************************************
 */

int rawRecvStatus( int iSocket, char *pcBuf) 
{
   char cModule[32]="rawRecvStatus";
   int iDebug = 0;

   int iRC, ii;
   int iBuf, iBufs;
   int iLen;
   char *pcc;
   srawStatus *pStatus;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   pcc = pcBuf;         /* points now to buffer in calling procedure */
   pStatus = (srawStatus *) pcc;
   memset(pcc, 0x00, sizeof(HEAD_LEN));
   iBuf = HEAD_LEN;
   iBufs = iBuf;

   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
      {
         if (iRC < 0) fprintf(fLogFile,
            "-E- %s: receiving status header\n", cModule);
         else
         {
            ii = iBufs - iBuf;
            fprintf(fLogFile,
               "-E- %s: connection to sender broken, %d byte of status header (%d byte) received\n",
               cModule, ii, iBufs);
         }
      
         if (errno)
         {
            fprintf(fLogFile, "    %s\n", strerror(errno));
            errno = 0;
         }

         return -1;
      }

      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   if (iBuf < 0) 
   {
      fprintf(fLogFile,
         "-E- %s: more status header data received than expected\n",
         cModule);
      return -2;
   }

   pStatus->iIdent = ntohl(pStatus->iIdent);
   pStatus->iStatus = ntohl(pStatus->iStatus);
   pStatus->iStatusLen = ntohl(pStatus->iStatusLen);

   if (iDebug)
   {
      fprintf(fLogFile, "    status header received (%d bytes)\n",
         iBufs);
      fprintf(fLogFile, "    ident %d, status %d, status len %d\n",
         pStatus->iIdent, pStatus->iStatus, pStatus->iStatusLen);
   }

   if (pStatus->iIdent != IDENT_STATUS)
   {
      fprintf(fLogFile, "-E- %s: invalid status header received (%d)\n",
              cModule, pStatus->iIdent);
      return -3;
   }

   iLen = pStatus->iStatusLen;
   if (iLen > 0)
   {
      memset(pcc, 0x00, (unsigned) iLen);
      iBuf = iLen;                      /* length of status message */
      iBufs += iBuf;
      while(iBuf > 0)
      {
         if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0 )
         {
            if (iRC < 0) fprintf(fLogFile,
               "-E- %s: receiving status message\n", cModule);
            else
            {
               ii = iLen - iBuf;
               fprintf(fLogFile,
                  "-E- %s: connection to sender broken, %d byte of status message (%d byte) received\n",
                  cModule, ii, iLen);
            }
      
            if (errno)
            {
               fprintf(fLogFile, "    %s\n", strerror(errno));
               errno = 0;
            }

            return -4;
         }

         iBuf -= iRC;
         pcc += iRC;

      } /* while(iBuf > 0) */

      if (iBuf < 0) 
      {
         fprintf(fLogFile,
            "-E- %s: more status data received than expected\n",
            cModule);
         return -5;
      }

      if (iDebug) fprintf(fLogFile,
         "    status message received (%d bytes):\n%s\n",
         iLen, pStatus->cStatus);

   } /*  iLen > 0 */

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iBufs;

} /* rawRecvStatus */

/*********************************************************************
 * rawSendRequest: send request buffer
 *    returns 0 or error (< 0)
 *    status header will be converted to net format
 *
 * created 21.12.2000, Horst Goeringer
 *********************************************************************
 */

int rawSendRequest(int iSocket, 
                   int iSeekMode,
                   int iOffset,
                   int iBufferSize)
{
   char cModule[32]="rawSendRequest";
   int iDebug = 0; 

   int iBuf, iRC;
   char *pcc;

   srawRequest sRequest; 
   int iRequSize = sizeof(srawRequest);

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: send request buffer\n", cModule);

   sRequest.iIdent = htonl((unsigned int) IDENT_NEXT_BUFFER);
   if (iSeekMode < 0)
      sRequest.iStatus = htonl(STA_NEXT_BUFFER);
   else
      sRequest.iStatus = htonl(STA_SEEK_BUFFER);
   sRequest.iStatusLen = htonl(iRequSize - HEAD_LEN);

   sRequest.iSeekMode = htonl(iSeekMode);
   sRequest.iOffset = htonl(iOffset);
   if (iBufferSize < 0)
   {
      fprintf(fLogFile,
         "-E- %s: invalid buffer size %d\n", cModule, iBufferSize);
      return -1;
   }
   sRequest.iBufferSize = htonl(iBufferSize);

   iBuf = iRequSize;
   pcc = (char *) &sRequest;
   iRC = send(iSocket, pcc, (unsigned) iBuf, 0);
   if (iRC < iBuf)
   {
      if (iRC < 0) fprintf(fLogFile,
         "-E- %s: sending request buffer\n", cModule);
      else fprintf(fLogFile,
         "-E- %s: request buffer incompletely sent (%d of %d byte)\n",
         cModule, iRC, iBuf);

      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

      return -1;
   }

   if (iDebug) fprintf(fLogFile,
      "-D- end %s: request buffer sent\n\n", cModule);

   return 0;

} /* rawSendRequest */

/*********************************************************************
 * rawSendStatus: send status buffer
 *    returns no. of bytes sent or error (< 0)
 *    status header will be converted to net format
 *
 * created  5. 3.96, Horst Goeringer
 *********************************************************************
 */

int rawSendStatus( int iSocket, int iStatus, char *pcMsg) 
{
   char cModule[32]="rawSendStatus";
   int iDebug = 0; 

   int iBuf = HEAD_LEN;
   int iRC, iMsgLen;
   char *pcc;
   srawStatus sStatus;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: send status buffer (%d) to socket %d\n",
      cModule, iStatus, iSocket);

   sStatus.iIdent = htonl((unsigned int) IDENT_STATUS);
   sStatus.iStatus = htonl(iStatus);
   sStatus.iStatusLen = htonl(0);

   if (pcMsg == NULL)
      iMsgLen = 0;
   else
      iMsgLen = strlen(pcMsg);
   if (iMsgLen)
   {
      iBuf += iMsgLen;
      sStatus.iStatusLen = htonl(iMsgLen);
      strcpy(sStatus.cStatus, pcMsg);
      if (iDebug) fprintf(fLogFile,
         "    %s: error message (%d bytes):\n    %s\n",
         cModule, iMsgLen, pcMsg);
   }

   pcc = (char *) &sStatus;
   if (iDebug)
   {
      fprintf(fLogFile,
         "    now send (iIdent %d, iStatus %d, iStatusLen %d), %d byte:\n",
         ntohl(sStatus.iIdent), ntohl(sStatus.iStatus),
         ntohl(sStatus.iStatusLen), iBuf);
      fflush(fLogFile);
   }

   iRC = send(iSocket, pcc, (unsigned) iBuf, 0);
   if (iRC < iBuf)
   {
      if (iRC < 0) fprintf(fLogFile,
         "-E- %s: sending status buffer\n", cModule);
      else fprintf(fLogFile,
         "-E- %s: status buffer incompletely sent (%d of %d byte)\n",
         cModule, iRC, iBuf);

      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

      return -1;
   }

   if (iDebug) fprintf(fLogFile,
      "-D- end %s: status buffer sent (%d byte)\n\n", cModule, iBuf);

   return(iBuf);

} /* rawSendStatus */

/*********************************************************************
 * rawTestFileName:
 *    verify that specified name is a valid file name
 *    and has no wildcards
 * created 15.3.1996, Horst Goeringer
 *********************************************************************
 */

int rawTestFileName( char *pcFile)
{
   char cModule[32] = "rawTestFileName";
   int iDebug = 0;

   int iRC;
   int ilen;
   int iError = 0;
   unsigned long lFileSize = 0;          /* dummy for rawGetFileSize */
   int iSize = 0;                        /* dummy for rawGetFileSize */
   char *pdir;

   if (iDebug) fprintf(fLogFile,
      "-D- begin %s: input file name %s\n", cModule, pcFile);

   if ( (pdir = strrchr(pcFile, '*')) != NULL) 
   {
      fprintf(fLogFile,
         "-E- invalid file name '%s': '*' not allowed as part of file name\n",
         pcFile);
      iError = 3;
      goto gErrorTest;
   }

   if ( (pdir = strrchr(pcFile, '?')) != NULL) 
   {
      fprintf(fLogFile,
         "-E- invalid file name '%s': '?' not allowed as part of file name\n",
         pcFile);
      iError = 3;
      goto gErrorTest;
   }

   if ( (pdir = strrchr(pcFile, '/')) == NULL) 
   {
      if (iDebug)
         fprintf(fLogFile, "    name %s okay\n", pcFile);
   }
   else       /* name contains '/' */ 
   {  
      ilen = strlen(pdir);
      if (iDebug) fprintf(fLogFile,
         "    trailor %s (len %d)\n", pdir, ilen);
      if (ilen == 1) 
      {
         strncpy(pdir, "\0", 1);
         if (iDebug)
            fprintf(fLogFile, "    %s is a directory\n", pcFile);
         iError = 2;
         goto gErrorTest;
      }
      else if (iDebug)
         fprintf(fLogFile, "    rel name %s okay\n", pcFile);
   }

   iRC = rawGetFileSize(pcFile, &lFileSize, &iSize);
   if (iDebug) fprintf(fLogFile,
      "    after rawGetFileSize, rc = %d\n", iRC);

   if (iRC)
   {
      if (iDebug)
      {
         fprintf(fLogFile, "-W- %s NOT archived", pcFile);
         if (iRC == 1)
            fprintf(fLogFile, " - is a directory\n");
         else if (iRC == 2)
            fprintf(fLogFile, " - is a symbolic link\n");  
         else if (iRC == 3)
            fprintf(fLogFile, " - is not a regular file\n");
         else
            fprintf(fLogFile, "\n");
      }

      iError = 2;
      goto gErrorTest;
   }

   if (iDebug) fprintf(fLogFile,
      "    %s is a regular file\n", pcFile);

gErrorTest:
   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iError;

} /* rawTestFileName */

