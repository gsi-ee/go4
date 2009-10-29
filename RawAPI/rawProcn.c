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

/**********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 14. 2.1996, Horst Goeringer
 **********************************************************************
 * rawProcn.c
 * utility programs for adsmcli package: all clients
 **********************************************************************
 * rawGetFileSize:   get file size (byte) via Posix call stat
 * rawGetFullFile:   get full file name from generic input & ll name
 * rawGetFSName:     get file space name from user identification
 * rawGetHLName:     get high level object name from path
 * rawGetLLName:     get low level object name from file name
 * rawGetPathName:   get path name from high level object name
 * rawQueryPrint:    print query results for one object to stdout
 * rawQueryString:   print query results for one object to string
 * rawRecvError:     receive error message
 * rawRecvHead:      receive common buffer header
 * rawRecvHeadC:     receive common buffer header and check
 * rawRecvRequest:   receive request for next buffer
 * rawRecvStatus:    receive status buffer
 * rawSendRequest:   send request for next buffer
 * rawSendStatus:    send status buffer
 * rawTestFileName:  verify that specified name is a valid file name
 * rawTestFilePath:  verify that file path not generic
 **********************************************************************
 * 16. 5.1997, H.G.: keep only modules for all clients here
 * 16. 6.1997, H.G.: ported to Linux
 * 15.10.1997, H.G.: rawCheckAuth moved to rawCheckAuth.c
 * 22.10.1997, H.G.: rawDelFilelist added
 *  8. 7.1998, H.G.: new filetype STREAM
 * 19.10.1998, H.G.: rawapitd.h introduced, dsmapitd.h removed
 * 27. 7.1999, H.G.: rawGetFullFile: don't check matches, rely on ADSM
 *  1.12.2000, H.G.: rawSendStatus: send also buffer request headers
 *  7. 2.2001, H.G.: new function rawQueryString
 * 18. 5.2001, H.G.: function rawDelFilelist -> rawcliproc.c
 * 18. 6.2001, H.G.: function rawTestFile: renamed to  rawTestFileName,
 *                   handle mixed case
 * 18. 7.2001, H.G.: use rawclin.h
 *  2.11.2001, H.G.: ported to W2000
 * 14.11.2001, H.G.: take alternate delimiter for Windows into account
 *  9. 1.2002, H.G.: rawGetLLName: pass object and delimiter as argument
 * 19. 2.2002, H.G.: rawGetFileSize added
 * 20. 2.2002, H.G.: rawGetCmdParms removed (unused)
 *  7. 3.2002, H.G.: rawRecvHeadC added
 *  7. 3.2002, H.G.: rawQueryPrint: new type GSI_MEDIA_INCOMPLETE
 *  7. 6.2002, H.G.: rawRecvHeadC: check always for error status
 * 23. 7.2002, H.G.: replace old printf() -> sprintf(fLogFile)
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
 **********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

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

#include <ctype.h>
#include <sys/socket.h>
#include <unistd.h>
#endif              /* end Unix */

#ifdef Linux
#include <linux/in.h>
#endif
#ifdef Lynx
#include <netinet/in.h>
#endif

#ifdef _AIX
#include <in.h>
#endif

#ifdef VMS
#include <in.h>
#include <unixio.h>
#include <unixlib.h>
#endif

#include "rawapitd.h"
#include "rawapitd-gsi.h"
#include "rawcommn.h"
#include "rawdefn.h"
#include "rawentn.h"

extern FILE *fLogFile;

static char cPath[MAX_FILE] = "";
static char cNamefs[MAX_OBJ_FS] = "";
static char cNamehl[MAX_OBJ_HL] = "";

static char *pcNull = "";        /* to be returned in case of error */
                       /* return(NULL) works in AIX, but not in VMS */

/**********************************************************************
 * rawGetFileSize
 *    get file size (bytes)
 * created 19.2.2002, Horst Goeringer
 **********************************************************************
 */

int rawGetFileSize(char *pcFile,
                   unsigned long *piSize,
                   unsigned long *piRecl)     /* needed only for VMS */
{
   char cModule[32] = "rawGetFileSize";
   int iDebug = 0;
   int iRC;
   unsigned long iRecl = 0;
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
         "-E- file %s unavailable\n", pcFile);
      perror("    ");
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
         "    file %s: size %d, recl %d (byte)\n",
         pcFile, iSize, iRecl);
   }
   else
   {
      if (pFileStatus->st_mode & S_IFDIR)
      {
         iRC = 1;
         if (iDebug) fprintf(fLogFile,
            "-W- %s is a directory, size %d\n", pcFile, iSize);
      }
#ifndef WIN32
#ifndef VMS
      else if (pFileStatus->st_mode & S_IFLNK)
      {
         iRC = 2;
         if (iDebug) fprintf(fLogFile,
            "-W- %s is a symbolic link, size %d\n", pcFile, iSize);
      }
#endif
#endif
      else
      {
         iRC = 3;
         if (iDebug) fprintf(fLogFile,
            "-W- unexpected item %s, size %d\n", pcFile, iSize);
      }
   }

   *piRecl = iRecl;
   *piSize = iSize;
   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iRC;

} /* rawGetFileSize */

/********************************************************************
 * rawGetFullFile: get full file name from generic input and ll name
 *    returns full file name
 *
 * created 18. 3.1996, Horst Goeringer
 ********************************************************************
 */

char *rawGetFullFile(char *pcFile, char *pcNamell)
{

   char cModule[32] = "rawGetFullFile";

   char cname1[MAX_FILE] = "";       /* name structure assumed:    */
   char cname2[MAX_FILE] = "";       /*    cPath / cname1 * cname2 */
                                     /* cPath globally declared    */

   int iDebug = 0;
   int ilen;
   int iRC;

   char *ploc=NULL, *pdelim=NULL, *pdelim2=NULL;
   char *pc, *pcll;

   strcpy(cPath, pcFile);
   pdelim = strrchr(cPath, *pcFileDelim);
   if (pdelim != NULL)                           /* path specified */
   {
      strncpy(pdelim, "\0", 1);
      strcpy(cname1, ++pdelim);
   }
   else
   {
#ifdef VMS
      pdelim2 = strrchr(cPath, *pcFileDelim2);
      if (pdelim2 != NULL)      /* logical device specified */
      {
         strncpy(pdelim2, "\0", 1);
         strcpy(cname1, ++pdelim2);
      }
      else
      {
#endif
         strncpy(cPath, "\0", 1);
         strcpy(cname1, pcFile);
#ifdef VMS
      }
#endif
   } /* (pdelim == NULL) */

   ilen = strlen(cname1);
   if (iDebug)
   {
#ifdef VMS
      if (pdelim != NULL)
         fprintf(fLogFile, "-D- %s: path %s], name1: %s (len %d)\n",
                 cModule, cPath, cname1, ilen);
            /* !!! bracket in fprintf needed, as not yet in cPath */
      else
      {
         if (pdelim2 != NULL)
            fprintf(fLogFile, "-D- %s: device %s, name1: %s (len %d)\n",
                    cModule, cPath, cname1, ilen);
         else
            fprintf(fLogFile, "-D- %s: no prefix, name1: %s (len %d)\n",
                    cModule, cname1, ilen);
      }
#else
      fprintf(fLogFile,
         "    path: %s, name1: %s (len %d)\n", cPath, cname1, ilen);
#endif
   }

   pc = &cname1[0];
   pcll = pcNamell;
   iRC = strncmp(pcll, pcObjDelim, 1);
   if (iRC == 0) pcll++;                  /* skip object delimiter */
   else
   {
      iRC = strncmp(pcll, pcObjDelimAlt, 1);
      if (iRC == 0) pcll++;               /* skip object delimiter */
   }

   if (strlen(cPath) > 0)
   {
#ifdef VMS
      if (pdelim2 != NULL)
         strcat(cPath, pcFileDelim2);
      else
#endif
         strcat(cPath, pcFileDelim);
   }
   strcat(cPath, pcll);
   if (iDebug) fprintf(fLogFile,
      "-D- end %s: full file name found: %s\n\n", cModule, cPath);

   return( (char *) cPath);

} /* rawGetFullFile */

/**********************************************************************
 * rawGetFSName
 *    get file space name from user specification
 * created 22.3.1996, Horst Goeringer
 **********************************************************************
 */

char *rawGetFSName( char *pcUser )
{
   int iDebug = 0;
   char cModule[32]="rawGetFSName";
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
              cNamefs, pcUser, pc, pcObjDelim);
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

} /* rawGetFSName */

/**********************************************************************/
/* rawGetHLName: get high level object name from path */
/* */
/* insert name part at beginning depending on device */
/* PLATFORM DEPENDENT */
/* */
/* created 16. 2.96, Horst Goeringer */
/* 23. 4.96., H.G: remove device dependency in hl-name, as in */
/*                 ADSM 213 management class may be set in dsmSendObj */
/**********************************************************************/

char *rawGetHLName( char *pcPath)
{
   int iDebug = 0;
   char cModule[32]="rawGetHLName";
   char *pdelim;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

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

   if (iDebug) fprintf(fLogFile,
      "-D- end %s: path: %s, hl: %s\n\n", cModule, pcPath, cNamehl);

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

int rawGetLLName( char *pcFile, char *pcDelimiter,
                  char *pcObject)
{
   char cModule[32] = "rawGetLLName";
   int iDebug = 0;

   char cNamell[MAX_OBJ_LL] = "";
   char *ploc = NULL;

   if (iDebug) fprintf(fLogFile,
      "-D- begin %s: object delim %s\n", cModule, pcDelimiter);

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
      "-D- end %s: file %s, object %s\n", cModule, pcFile, pcObject);

   return 0;

} /* rawGetLLName */

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
      fprintf(fLogFile,
            "-E- %s: high level name empty\n");
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

/*********************************************************************
 * rawQueryPrint: print query results for one object
 * created 19. 2.1996, Horst Goeringer
 *********************************************************************
 */

void rawQueryPrint(
        srawObjAttr *pQAttr,
        int ipMode) /* =  0: default print mode,
                       =  1: debug print mode,
                       = 10: default print mode, stage status unknown
                       = 11: debug print mode, stage status unknown  */
{
   char ctype[8] = "";
   char cMClass[12] = "";
   char cPath[MAX_OBJ_HL];
   char cStatus[16] = "";
   int iStage = 0;                         /* StagePool (read cache) */
   int iCache = 0;                      /* ArchivePool (write cache) */

   switch( ntohl(pQAttr->iFileType) )
   {
      case FIXED_INTS4:
         strcpy(ctype, "FB");
         break;
      case STREAM:
         strcpy(ctype, "U ");
         break;
      default:
         fprintf(fLogFile, "-E- Invalid record format %d found\n",
                 ntohl(pQAttr->iFileType));
         break;
   }

   switch( ntohl(pQAttr->iMediaClass) )
   {
      case MEDIA_FIXED:
         strcpy(cMClass, "DISK");
         break;
      case MEDIA_LIBRARY:
         strcpy(cMClass, "TAPE");
         break;
      case MEDIA_NETWORK:
         strcpy(cMClass, "NETWORK");
         break;
      case MEDIA_SHELF:
         strcpy(cMClass, "SHELF");
         break;
      case MEDIA_OFFSITE:
         strcpy(cMClass, "OFFSITE");
         break;
      case MEDIA_UNAVAILABLE:
         strcpy(cMClass, "UNAVAILABLE");
         break;
      case GSI_MEDIA_STAGE:
      case GSI_MEDIA_LOCKED:
         strcpy(cMClass, "STAGE");
         strcpy(cStatus, "staged");
         iStage = 1;
         break;
      case GSI_MEDIA_INCOMPLETE:             /* keep TSM media class */
         strcpy(cMClass, "TAPE");
         break;
      case GSI_MEDIA_CACHE:                       /* GSI write cache */
         strcpy(cMClass, "CACHE");
         strcpy(cStatus, "cached");
         iCache = 1;
         break;
      default:
         fprintf(fLogFile, "-E- Invalid media class %d found\n",
                 ntohl(pQAttr->iMediaClass));
         break;
   }

   if ( (ipMode == 10) || (ipMode == 11) )
      strcat(cMClass, "??");             /* stage status not checked */

   if (iCache)
      strcat(pQAttr->cDate, " ");
   strcpy(cPath, rawGetPathName(pQAttr->cNamehl));
   fprintf(fLogFile, "%s%s%s  %s  %s %s %d  %s",
           pQAttr->cNamefs,
           cPath,
           pQAttr->cNamell,
           pQAttr->cOwner,
           pQAttr->cDate,
           ctype,
           ntohl(pQAttr->iBufsizeFile),
           cMClass);
   if (ntohl(pQAttr->iFileSize))
      fprintf(fLogFile, "  %d", ntohl(pQAttr->iFileSize) );
   if (iStage)
      fprintf(fLogFile, "  %s  %s", pQAttr->cNode, pQAttr->cStageUser);
   if (iCache)
      fprintf(fLogFile, "  %s", pQAttr->cNode);
   fprintf(fLogFile, "\n");

   if ( (ipMode == 1) || (ipMode == 11) )
   {
      if (ntohl(pQAttr->iFS))
         fprintf(fLogFile, "    %s on data mover %s, FS %d\n",
                 cStatus, pQAttr->cNode, ntohl(pQAttr->iFS));
      fprintf(fLogFile, "    obj-Id: %lu-%lu, restore order: %lu-%lu\n",
              ntohl(pQAttr->iObjHigh),
              ntohl(pQAttr->iObjLow),
              ntohl(pQAttr->iRestoHigh),
              ntohl(pQAttr->iRestoLow) );
      fprintf(fLogFile,
              "    version %d, owner: %s, OS: %.8s, mgmt-class: %s\n",
              ntohl(pQAttr->iVersion),
              pQAttr->cOwner,
              pQAttr->cOS,
              pQAttr->cMgmtClass);
   }

} /* rawQueryPrint */

/*********************************************************************/
/* rawQueryString: print query results for one object to string */
/* created 19. 2.96, Horst Goeringer */
/*********************************************************************/

int rawQueryString(
        srawObjAttr *pQAttr,
        int ipMode, /* =  0: default print mode,
                       =  1: debug print mode,
                       = 10: default print mode, stage status unknown
                       = 11: debug print mode, stage status unknown  */
        char *pcOut)
{
   int iDebug = 0;
   char cModule[32] = "rawQueryString";

   int iOut = 0;
   int iMsg = 0;
   char ctype[8] = "";
   char cMClass[12] = "";
   char cPath[MAX_OBJ_HL];
   char cMsg[512] = "";
   char cMsg1[512] = "";

   int iStage = 0;                         /* StagePool (read cache) */
   int iCache = 0;                      /* ArchivePool (write cache) */
   switch( ntohl(pQAttr->iFileType) )
   {
      case FIXED_INTS4:
         strcpy(ctype, "FB");
         break;
      case STREAM:
         strcpy(ctype, "U ");
         break;
      default:
         fprintf(fLogFile, "-E- Invalid record format %d found\n",
                 ntohl(pQAttr->iFileType));
         break;
   }

   switch( ntohl(pQAttr->iMediaClass) )
   {
      case MEDIA_FIXED:
         strcpy(cMClass, "DISK");
         break;
      case MEDIA_LIBRARY:
         strcpy(cMClass, "TAPE");
         break;
      case MEDIA_NETWORK:
         strcpy(cMClass, "NETWORK");
         break;
      case MEDIA_SHELF:
         strcpy(cMClass, "SHELF");
         break;
      case MEDIA_OFFSITE:
         strcpy(cMClass, "OFFSITE");
         break;
      case MEDIA_UNAVAILABLE:
         strcpy(cMClass, "UNAVAILABLE");
         break;
      case GSI_MEDIA_STAGE:
      case GSI_MEDIA_LOCKED:
         strcpy(cMClass, "STAGE");
         iStage = 1;
         break;
      case GSI_MEDIA_INCOMPLETE:             /* keep TSM media class */
         strcpy(cMClass, "TAPE");
         break;
      case GSI_MEDIA_CACHE:                       /* GSI write cache */
         strcpy(cMClass, "CACHE");
         iCache = 1;
         break;
      default:
         fprintf(fLogFile, "-E- Invalid media class %d found\n",
                 ntohl(pQAttr->iMediaClass));
         break;
   }

   if ( (ipMode == 10) || (ipMode == 11) )
      strcat(cMClass, "??");             /* stage status not checked */

   strcpy(cPath, rawGetPathName(pQAttr->cNamehl));
   sprintf(cMsg, "%s%s%s  %s  %s %s %d  %s",
                  pQAttr->cNamefs,
                  cPath,
                  pQAttr->cNamell,
                  pQAttr->cOwner,
                  pQAttr->cDate,
                  ctype,
                  ntohl(pQAttr->iBufsizeFile),
                  cMClass);

   if (ntohl(pQAttr->iFileSize))
   {
      sprintf(cMsg1, "  %d", ntohl(pQAttr->iFileSize) );
      strcat(cMsg, cMsg1);
   }
   if (iStage)
   {
      sprintf(cMsg1, "  %s  %s", pQAttr->cNode, pQAttr->cStageUser);
      strcat(cMsg, cMsg1);
   }
   if (iCache)
   {
      sprintf(cMsg1, "  %s", pQAttr->cNode);
      strcat(cMsg, cMsg1);
   }
   strcat(cMsg, "\n");

   if ( (ipMode == 1) || (ipMode == 11) )
   {
      if (ntohl(pQAttr->iFS))
         fprintf(fLogFile, "    staged on data mover %s, FS %d\n",
                 pQAttr->cNode, ntohl(pQAttr->iFS));
      sprintf(cMsg1, "    obj-Id: %lu-%lu, restore order: %lu-%lu\n",
                  ntohl(pQAttr->iObjHigh),
                  ntohl(pQAttr->iObjLow),
                  ntohl(pQAttr->iRestoHigh),
                  ntohl(pQAttr->iRestoLow) );
      strcat(cMsg, cMsg1);
      sprintf(cMsg1, "    version %d, owner: %s, OS: %s, mgmt-class: %s\n",
                  ntohl(pQAttr->iVersion),
                  pQAttr->cOwner,
                  pQAttr->cOS,
                  pQAttr->cMgmtClass);
      strcat(cMsg, cMsg1);
   }

   iOut = strlen(pcOut);
   iMsg = strlen(cMsg);
   if (iDebug) fprintf(fLogFile,
      "-D- %s: message length %d byte, returned %d byte\n",
      cModule, iMsg, iOut);

   if (iOut < iMsg)
   {
      fprintf(fLogFile,
         "-W- %s: query information incomplete\n", cModule);
      fprintf(fLogFile,
         "    %s: output string provided too short (%d byte), need %d\n",
        cModule, iOut, iMsg);
      strncpy(pcOut, cMsg, iOut);
      return iOut;
   }

   strcpy(pcOut, cMsg);
   return iMsg;

} /* rawQueryString */

/*********************************************************************/
/* rawRecvError:   receive error message */
/*    returns length of error message */
/* */
/* created 14. 3.96, Horst Goeringer */
/*********************************************************************/

int rawRecvError( int iSocket, int iLen, char **pMsg)
{
   int iDebug = 0;
   char cModule[32]="rawRecvError";
   int iRC;
   int iBuf, iBufs;
   char *pcMsg;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   pcMsg = *pMsg;       /* points now to buffer in calling procedure */
   iBuf = iLen;                           /* length of error message */
   iBufs = iBuf;
   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcMsg, iBuf, 0 )) < 0 )
      {
         perror("-E- rawRecvError: receiving error message");
         return -1;
      }
      iBuf -= iRC;
      pcMsg += iRC;

   } /* while(iBuf > 0) */

   if (iBuf < 0)
   {
      fprintf(fLogFile,
         "-E- %s: more error data received than expected\n", cModule);
      return -2;
   }

   *pcMsg = '\0';                          /* delimit message string */

   if (iDebug) fprintf(fLogFile,
      "    end %s: error message received (%d bytes)\n    %s\n",
      cModule, iBufs, *pMsg);

   return iBufs;

} /* rawRecvError */

/*********************************************************************
 * rawRecvHead: receive common buffer header
 *    returns no. of bytes received or error (< 0)
 *
 * created  5. 3.96, Horst Goeringer
 *********************************************************************
 */

int rawRecvHead( int iSocket, char **pBuf)
{
   int iDebug = 0;
   char cModule[32]="rawRecvHead";

   int iBuf, iBufs;
   int iRC;
   int iIdent;
   int iStatus;
   int iDataLen;
   char *pcc;
   int *pint;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   pcc = *pBuf;        /* points now to buffer in calling procedure */
   pint = (int *) pcc;
   iBuf = HEAD_LEN;
   iBufs = iBuf;
   while(iBuf > 0)
   {
      iRC = recv( iSocket, pcc, iBuf, 0 );
      if (iRC <= 0)
      {
         if (iRC < 0)
         {
            fprintf(fLogFile, "-E- %s: rc(recv) = %d\n", cModule, iRC);
            perror("-E- rawRecvHead: receiving buffer header");
         }
         else
            fprintf(fLogFile, "-E- %s: connection to client broken\n",
                   cModule);

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
                 char *pBuf,
                 int iIdentReq,     /* < 0 => check, = 0 => no check */
                 int iStatusReq,   /* >= 0 => check, < 0 => no check */
                 char *pcMsg)
{
   int iDebug = 0;
   char cModule[32] = "rawRecvHeadC";

   /* header to be received */
   int iIdent;
   int iStatus;
   int iDataLen;

   char cMsg[256] = "";          /* for error message to be received */

   int iRC;
   int iBuf, iBufs;
   char *pcc;
   int *pint;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   pcc = pBuf;          /* points now to buffer in calling procedure */
   iBuf = HEAD_LEN;
   iBufs = iBuf;
   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcc, iBuf, 0 )) <= 0 )
      {
         if (iRC < 0)
         {
            perror("-E- rawRecvHeadC: receiving buffer header");
            sprintf(pcMsg, "-E- %s: receiving buffer header\n",
                    cModule);
            return -1;
         }
         else
         {
            sprintf(pcMsg, "-E- %s: connection to sender lost\n",
                    cModule);
            return -2;
         }
      }
      iBuf -= iRC;
      pcc += iRC;

   } /* while(iBuf > 0) */

   if (iDebug) fprintf(fLogFile,
      "    buffer header received (%d bytes)\n", iBufs);

   pint = (int *) pBuf;
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
         fprintf(fLogFile, "    status received\n");

      if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) )
      {
         if (iDebug)
            fprintf(fLogFile, "    error received\n");
         /* sprintf(pcMsg, "-E- %s: received error status:\n",
                 cModule); */

         if (iDataLen)
         {
            pcc = cMsg;
            iRC = rawRecvError(iSocket, iDataLen, &pcc);
            if (iRC < 0)
            {
               sprintf(pcMsg, "-E- %s: receiving error msg, rc=%d\n",
                       cModule, iRC);
               return -3;
            }

            if (iDebug) fprintf(fLogFile,
               "    msg (%d byte): %s\n", iDataLen, pcc);
            strcat(pcMsg, pcc);
            return strlen(pcMsg);
         }
         else
         {
            strcat(pcMsg,
               "-W- error status received, but no error message available\n");
            return 0;
         }

      } /* (iStatus == STA_ERROR || STA_ERROR_EOF) */
   } /* iIdent == IDENT_STATUS */

   if (iIdentReq < 0)                /* test for expected identifier */
   {
      if (iDebug)
         fprintf(fLogFile, "    check identifier\n");

      if (iIdent != iIdentReq)
      {
         sprintf(pcMsg,
                 "-E- %s: unexpected header (ident %d) received\n",
                 cModule, iIdent);
         return -4;

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
               return -5;
            }
         }
      } /* expected ident */
   } /* check ident requested */

   return HEAD_LEN;

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
   int iDebug = 0;
   char cModule[32]="rawRecvRequest";

   int iRC;
   int iRequSize = sizeof(srawRequest);
   int iBuf, iBuf0;
   char *pc;
   char cMsg[256] = "";                       /* error msg received */

   srawRequest sRequest, *pRequest;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: receive request buffer\n", cModule);

   pRequest = &sRequest;
   pc = (char *) pRequest;
   iBuf = HEAD_LEN;
   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pc, iBuf, 0 )) < 0 )
      {
         perror("-E- rawRecvRequest: receiving buffer header");
         return -1;
      }
      iBuf -= iRC;
      pc += iRC;
   } /* while(iBuf > 0) */

   if (iBuf < 0)
   {
      fprintf(fLogFile,
         "-E- %s: more header data received than expected\n", cModule);
      return -2;
   }

   pRequest->iIdent = ntohl(pRequest->iIdent);
   if (iDebug) fprintf(fLogFile,
      "-D- %s: buffer header received (HEAD_LEN bytes, id %d)\n",
      cModule, pRequest->iIdent);

   if ( (pRequest->iIdent != IDENT_NEXT_BUFFER) &&
        (pRequest->iIdent != IDENT_STATUS) )
   {
      fprintf(fLogFile, "-E- %s: invalid buffer received (id %d)\n",
              cModule, pRequest->iIdent);
      return -3;
   }

   pRequest->iStatus = ntohl(pRequest->iStatus);
   pRequest->iStatusLen = ntohl(pRequest->iStatusLen);
   iBuf0 = pRequest->iStatusLen;
   iBuf = iBuf0;
   if (pRequest->iIdent != IDENT_NEXT_BUFFER)
      pc = cMsg;                               /* more space needed */
   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pc, iBuf, 0 )) < 0 )
      {
         perror("-E- rawRecvRequest: receiving buffer data");
         return -1;
      }
      iBuf -= iRC;
      pc += iRC;
   } /* while(iBuf > 0) */

   if (iBuf < 0)
   {
      fprintf(fLogFile, "-E- %s: more data received than expected\n",
              cModule);
      return -2;
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
         return -4;
      }

      *piSeekMode = ntohl(pRequest->iSeekMode);
      *piOffset = ntohl(pRequest->iOffset);
      *piBufferSize = ntohl(pRequest->iBufferSize);

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
         if (iBuf0 > 0)
            fprintf(fLogFile,
               "-W- %s: error message from client:\n    %s\n",
               cModule, cMsg);
         return -4;
      }
      else
      {
         fprintf(fLogFile,
            "-E- %s: invalid status buffer received (id %d)\n",
            cModule, pRequest->iStatus);
         return -3;
      }

      return pRequest->iStatus;

   } /* (pRequest->iIdent == IDENT_STATUS) */
   else
   {
      fprintf(fLogFile, "-E- %s: invalid buffer received (ident %d)\n",
              cModule, pRequest->iIdent);
      return -2;
   }


} /* rawRecvRequest */

/*********************************************************************
 * rawRecvStatus: receive status header
 *    returns no. of bytes received or error (< 0)
 *    status header will be converted to host format
 *
 * created 18. 3.96, Horst Goeringer
 *********************************************************************
 */

int rawRecvStatus( int iSocket, char **pBuf)
{
   int iDebug = 0;
   char cModule[32]="rawRecvStatus";

   int iRC;
   int iBuf, iBufs;
   int iLen;
   char *pcc;
   srawStatus *pStatus;

   pcc = *pBuf;    /* points now to buffer in calling procedure */
   pStatus = (srawStatus *) pcc;
   memset(pcc, 0x00, sizeof(HEAD_LEN));
   iBuf = HEAD_LEN;
   iBufs = iBuf;

   while(iBuf > 0)
   {
      if ( (iRC = recv( iSocket, pcc, iBuf, 0 )) < 0 )
      {
         fprintf(fLogFile, "-E- %s: receiving status header\n",
                 cModule);
         perror("-E- rawRecvStatus: receiving status header");
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
   if (pStatus->iIdent != IDENT_STATUS)
   {
      fprintf(fLogFile, "-E- %s: invalid status header received (%d)\n",
              cModule, pStatus->iIdent);
      return -3;
   }

   if (iDebug) fprintf(fLogFile,
      "    %s: status header received (status %d, %d bytes)\n",
      cModule, pStatus->iStatus, iBufs);

   iLen = pStatus->iStatusLen;
   if (iLen > 0)
   {
      memset(pcc, 0x00, iLen);
      iBuf = iLen;                      /* length of status message */
      iBufs += iBuf;
      while(iBuf > 0)
      {
         if ( (iRC = recv( iSocket, pcc, iBuf, 0 )) < 0 )
         {
            perror("-E- rawRecvStatus: receiving status message");
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
         "    %s: status message received (%d bytes)\n", cModule, iLen);

   } /*  iLen > 0 */

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
   int iDebug = 0;
   char cModule[32]="rawSendRequest";

   int iBuf, iRC;
   char *pcc;

   srawRequest sRequest;
   int iRequSize = sizeof(srawRequest);

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: send request buffer\n", cModule);

   sRequest.iIdent = htonl(IDENT_NEXT_BUFFER);
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
   if ( (iRC = send( iSocket, pcc, iBuf, 0 )) < 0 )
   {
      perror("-E- rawSendRequest: sending request buffer");
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
   int iDebug = 0;
   char cModule[32]="rawSendStatus";

   int iBuf = HEAD_LEN;
   int iRC, iMsgLen;
   char *pcc;
   srawStatus sStatus;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: send status buffer (%d) to socket %d\n",
      cModule, iStatus, iSocket);

   sStatus.iIdent = htonl(IDENT_STATUS);
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
   if ( (iRC = send( iSocket, pcc, iBuf, 0 )) < 0 )
   {
      perror("-E- rawSendStatus: sending status buffer");
      return -1;
   }

   if (iDebug) fprintf(fLogFile,
      "-D- end %s: status buffer sent (%d byte)\n\n", cModule, iBuf);

   return(iBuf);

} /* rawSendStatus */

/*********************************************************************
 * rawTestFileName: verify that specified name is a valid file name
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
   unsigned long iFileSize, iSize;     /* dummies for rawGetFileSize */

   char cFile0[MAX_FILE] = "";
   char *pdir;

   if (iDebug) fprintf(fLogFile,
      "-D- begin %s: input file name %s\n", cModule, pcFile);

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

   iRC = rawGetFileSize(pcFile, &iFileSize, (unsigned long *) &iSize);
   if (iDebug) printf(
      "    after rawGetFileSize, rc = %d\n", iRC);

   if (iRC)
   {
      if (iDebug)
      {
         printf("-W- %s NOT archived", pcFile);
         if (iRC == 1)
            printf(" - is a directory\n");
         else if (iRC == 2)
            printf(" - is a symbolic link\n");
         else if (iRC == 3)
            printf(" - is not a regular file\n");
         else
            printf("\n");
      }

      iError = 2;
      goto gErrorTest;
   }

   if (iDebug) printf(
      "    %s is a regular file\n", pcFile);

gErrorTest:
   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iError;

} /* rawTestFileName */

/********************************************************************/
/* rawTestFilePath: verify that file path not generic */
/*                  if tape specified get tape drive name */
/*    returns 0 if file name okay */
/* */
/* created 15. 3.96, Horst Goeringer */
/********************************************************************/

int rawTestFilePath( char *pcFile0, char **pcFile, char **pcTape)
{

   int iDebug = 0;
   char cModule[32]="rawTestFilePath";

   int iloc, irc;
   int imax = 0;
   int iDevType = 1;                     /* default: disk */

   bool_t bup = bFalse;

   char cText[12] = "", ctemp[MAX_FILE] = "";
   char *plocd = NULL, *plocs = NULL, *ploc = NULL;
   char *pdelim = NULL, *pgen = NULL;
   char *pcSemi = ";";
   char *pcc, *pBuft, *pBuff;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: test file %s\n", cModule, pcFile0);

   /* check if device without file name specified */
   pBuft = *pcTape;
   pBuff = *pcFile;
   strcpy(ctemp, pcFile0);
   plocd = strchr(ctemp, *pcDevDelim);
   if (plocd != NULL)
   {
      iDevType = 2;        /* tape, if Unix; if VMS: get it later */
      pcc = plocd;
      irc = strncmp(++pcc, "\0", 1);
      if (irc == 0)
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
   plocs = strchr(ctemp, *pcSemi);
   if (plocs)
   {
      ++plocs;
      irc = isalnum(*plocs);
      if (irc)                      /* char after semicolon alphanum. */
      {
         irc = isalpha(*plocs);
         if (irc == 0)              /* char after semicolon no letter */
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
      strcpy(pcFile0, ctemp);
   }

   iDevType = rawCheckDevice(ctemp, pcFile, pcTape);
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

   if (iDevType == 2)               /* tape */
   {
#ifndef VMS
      irc = strncmp(pcc, pcDevDelim, 1);
      if (irc == 0)
      {
         fprintf(fLogFile,
            "-E- node specification not allowed in file name: %s\n",
            pcFile0);
         return(-2);
      }
#endif /* Unix */

      strncpy(plocd++, "\0", 1);    /* cut string at colon */
      strcpy(pBuft, ctemp);
#ifdef VMS
      strncat(pBuft, pcDevDelim, 1); /* append colon in VMS */

#else
      pgen = strchr(pBuft, *pcStar);
      if (pgen != NULL)
      {
         fprintf(fLogFile, "-E- specified device %s has generic path\n",
                 pBuft);
         fprintf(fLogFile,
            "    only the relative file name may be generic\n");
         return(-1);
      }
#endif

      strcpy(pBuff, plocd);
      if (iDebug)
         fprintf(fLogFile, "    file %s on tape %s\n", pBuff, pBuft);
      strcpy(cText, "tape");
      irc = 1;

   } /* (iDevType == 2) */
   else
   {
      strcpy(pBuff, ctemp);
      strcpy(cText, "disk");
      irc = 0;
   }

   if (iDebug) fprintf(fLogFile,
      "    rawTestFilePath: %s file %s\n", cText, pBuff);

   pdelim = strrchr(pBuff, *pcFileDelim);
   if (pdelim != NULL)                             /* path specified */
   {
#ifndef VMS
      if (iDevType == 2)                             /* file on tape */
      {
         fprintf(fLogFile,
            "-E- path in tape file name not allowed: %s\n", pBuff);
         return(-1);
      }
#endif

      pgen = strchr(pBuff, *pcStar);
      if ( (pgen != NULL) && (pgen < pdelim) )
      {
         fprintf(fLogFile,
            "-E- specified %s file %s has generic path\n",
            cText, pBuff);
         fprintf(fLogFile,
            "    only the relative file name may be generic\n");
         return(-1);
      }

      pdelim++;                          /* points to rel. file name */

   } /* (pdelim != NULL) */
   else pdelim = pBuff;

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
      "    rawTestFilePath: file name %s okay\n", pcFile0);

   return(irc);

} /* rawTestFilePath */



