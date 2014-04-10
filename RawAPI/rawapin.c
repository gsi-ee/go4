/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created  4.12.2000 by Horst Goeringer
 ********************************************************************
 * rawapin.c
 *   procedures for RFIO API client
 *   new version (corresponds to tsmcli)
 *
 * fully implemented (! GSI extension):
 *
 * rfio_open_gsidaq: ! open remote file in gStore and lustre
 * rfio_open:       open remote file in GSI mass storage
 * rfio_endfile:    ! close remote file, but keep connection to DM
 * rfio_newfile:    ! continue with next remote file in GSI mass storage
 * rfio_read:       read remote file in GSI mass storage
 * rfio_write:      write remote file in GSI mass storage
 * rfio_close:      close remote file and connection to GSI mass storage
 * rfio_lseek:      move read/write file pointer in remote file
 * rfio_stat:       get file status (32 bit file size)
 * rfio_stat64:     get file status (64 bit file size)
 * rfio_cache_stat: ! returns cache status of file
 *
 * rfio_fopen_gsidaq_dm: ! open remote file in gStore and lustre,
 *                         provide DM name and no.
 * rfio_fopen_gsidaq: ! open remote file in gStore and lustre
 * rfio_fopen:      open remote file in GSI mass storage
 * rfio_fendfile:   ! close remote file, but keep connection to DM
 * rfio_fnewfile:   ! continue with next remote file in GSI mass storage
 * rfio_fread:      read remote file in GSI mass storage
 * rfio_fwrite:     write remote file in GSI mass storage
 * rfio_fclose:     close remote file and connection to GSI mass storage
 * rfio_fstat:      get file status (32 bit file size)
 * rfio_fstat64:    get file status (64 bit file size)
 *
 * implemented only as dummy:
 *
 * rfio_open64:     needed, calls rfio_open
 * rfio_lseek64:    needed, calls rfio_lseek
 * rfio_preseek64: 
 * rfio_mkdir:      not needed, implicitly done
 * rfio_opendir:    not needed, implicitly done
 * rfio_closedir:   not needed, implicitly done 
 * rfio_readdir:
 *
 * rfio_access:     get access status
 * rfio_parse:      parse name to host and path
 * rfio_perror:     print error message to stderr
 * rfio_serror:     print last error message to string
 * rfio_unlink:
 * rfiosetopt: 
 *
 **********************************************************************
 *  5. 8.2002, H.G.: new argument list in rawGetLLName
 *  6. 8.2002, H.G.: connect to master server and data mover
 *                   enhanced error handling
 *  8. 8.2002, H.G.: shutdown of server processes after CTL C
 *  9. 9.2002, H.G.: rfiosetopt added
 * 17. 9.2002, H.G.: server node name added
 * 23. 9.2002, H.G.: RFIO write functions added (for RFIO disk server)
 * 14.10.2002, H.G.: ported to Lynx
 * 18. 3.2003, H.G.: rawdef.h -> rawdefn.h
 * 18. 7.2003, H.G.: remove CTL_C-handling
 * 28. 8.2003, H.G.: add rfio_closedir, rfio_mkdir, rfio_opendir,
 *                   rfio_readdir, rfio_stat as dummies
 * 25. 9.2003, H.G.: take synchronization into account
 * 29. 4.2004, H.G.: catch action without open in advance
 * 16. 7.2004, H.G.: new entries: rfio_[f]endfile, rfio_[f]newfile
 *  4. 8.2004, H.G.: new entries for rfiodaq
 * 18. 8.2004, H.G.: new entries for rfiocopy
 * 22.11.2004, H.G.: implement rfio_fstat
 * 25.11.2004, H.G.: implement rfio_stat
 * 26. 1.2005, H.G.: rawapitd-gsi.h -> rawapitd-gsin.h, new media names:
 *                   GSI_CACHE_LOCKED, GSI_CACHE_INCOMPLETE
 * 10. 5.2005, H.G.: handle data mover switch
 * 16.12.2005, H.G.: rename rawGetFSName to rawGetFSpName
 *  3. 4.2006, H.G.: ported to sarge
 * 18. 4.2006, H.G.: new entry server lxgstore
 * 25. 4.2006, H.G.: read via rfiocopy implemented
 *  9. 5.2006, H.G.: rename QUERY_RETRIEVE_API -> QUERY_RETRIEVE_RECORD
 *  1.11.2006, H.G.: iMode=1: no connection to DM (only query)
 *  2.11.2006, H.G.: remove explicit query on alternate host:
 *                   is done automatically on entry server
 * 16. 3.2007, H.G.: handle data of both ATL servers
 * 18. 4.2007, H.G.: new prefix node 'gstore', no longer 'adsmexp'
 * 22. 5.2007, H.G.: new entries rfio_open64, rfio_lseek64
 * 29. 5.2007, H.G.: new entries rfio_stat64, rfio_fstat64
 *  1. 6.2007, H.G.: iMode=-1: no connection to DM (only query)
 *                   iMode>0 wird von Root gesetzt (file permissions)
 *                   add externals for Root 512-00
 *  5. 7.2007, H.G.: rfio_access implemented
 * 23. 8.2007, H.G.: writing: handling of DM session in file loop 
 * 24. 8.2007, H.G.: use iPoolIdRC, iPoolIdWC in srawComm
 * 28. 8.2007, H.G.: use fprintm in Lynx,
 *                   remove length restriction for obj name
 * 25. 4.2008, H.G.: exclude usage of central data FS
 * 16. 5.2008, H.G.: rfio_open, rfio_newfile: remove './' and
 *                   trailing '/.' in gStore path name
 * 29. 5.2008, H.G.: new entry rfio_preseek64 (dummy version)
 *  5.11.2008, H.G.: rfio_open, rfio_read:
 *                      improved error handling after rc=0 from recv()
 *                   calling rawRecv...(): arg char ** -> char *
 * 11.11.2008, H.G.: add suggestions of Hakan
 * 12.11.2008, H.G.: new entry rfio_cache_stat implemented
 * 13.11.2008, H.G.: inhibit using rfio_newfile/rfio_endfile for read
 * 17.11.2008, H.G.: mod file args from 'char *' to 'const char *' in:
 *                   rfio_access, rfio_fwrite, rfio_open, rfio_open64,
 *                   rfio_unlink, rfio_write 
 *  3.12.2008, H.G.: add suggestions of Hakan, part II
 * 10. 2.2009, H.G.: new entries rfio_fopen_gsidaq, rfio_open_gsidaq
 * 26. 6.2009, H.G.: for 64 bit client: replace int args by size_t:
 *                   rfio_fread: args 2, 3
 *                   rfio_fwrite: args 2, 3
 *                   rfio_read: arg 3
 *                   rfio_write: arg 3
 * 21. 8.2009, H.G.: rfio_newfile: optimize handling of large buflen
 *  6.11.2009, H.G.: rfio_cache_stat: support call in loop
 *  9.11.2009, H.G.: rfio_open_gsidaq: close if open for query only
 *                   iOnlyQuery now global, no more DM selection if set
 *                   rfio_cache_stat, rfio_stat: no more close needed
 *  4.12.2009, H.G.: handle enhanced structure srawObjAttr
 * 29. 1.2010, H.G.: enhance 256 -> STATUS_LEN for:
 *                      cMsg, cTemp, cPrefix, rfio_errmsg
 * 15. 2.2010, H.G.: add flag __USE_LARGEFILE64 for 64 bit functions
 * 25. 2.2010, H.G.: rfio_cache_stat, rfio_stat:
 *                      improve handling of already open files
 *                   rfio_open_gsidaq: 
 *                      inhibit handling of already open files
 * 26. 2.2010, H.G.: rawQueryString: add parameter (len output string)
 *                   rfio_gsi_query: add parameter (len output string)
 *  9. 3.2010, H.G.: take entry server name from URL specified 
 * 16. 4.2010, H.G.: srawDataMoverAttr: iSynchId -> iATLServer
 * 23. 4.2010, H.G.: new entry rfio_fopen_gsidaq_dm
 *                   rfio_open_gsidaq: clean control block after error
 *                      copy fraction 0 specified: assume 1
 * 22. 9.2010, H.G.: handle nodename 'lxgstore.gsi.de'
 * 24. 9.2010, H.G.: handle 64 bit filesizes, now also in
 *                      rfio_fstat64, rfio_stat64
 * 23.11.2010, H.G.: gStore files >=4GB: inhibit read, incomplete query
 **********************************************************************
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#ifdef Linux
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

FILE *fLogClient = NULL;                               /* client log */
#define MAX_LOG_FILE 64        /* max length of client log file name */
static char cLogClient[MAX_LOG_FILE] = "";
static int iLogFile = 1;              /* default: client log to file */
static int iOS64 = 0;    /* =0: 4 byte filesize, =1: 8 byte filesize */

#ifdef Lynx
#include "error_mac.h"
#include "errnum_def.h"
#include "err_mask_def.h"
#include "f_ut_printm.h"

#define fprintf  fprintm

#endif

#include "rawdefn.h"
#include "rawapin.h"    /* has includes for rawcommn.c and rawclin.h */
#include "rawentn.h"

#include "rawapitd.h"
#include "rawapitd-gsin.h"
#include "rawapplcli.h"

int* piEntryList;              /* external in some utility functions */
int imySigS;         /* if = 1: CTL C specified (extern in rconnect) */
FILE *fLogFile = NULL;         /* external in some utility functions */

/* externals for ROOT: TRFIOFile.cxx */
int serrno;                               /* id system error message */ 
int rfio_errno;                             /* id RFIO error message */
int tStageHostKey;                            /* Castor, root 512-00 */
int tStagePortKey;                            /* Castor, root 512-00 */
int tSvcClassKey;                             /* Castor, root 512-00 */
int tCastorVersionKey;                        /* Castor, root 512-00 */

char *rawGetFSpName(char *);

static char rfio_errmsg[STATUS_LEN] = "";          /* msg for stderr */ 

#define MAX_BUFSIZE 32768            
static int iint = sizeof(int);
static int irawComm = sizeof(srawComm);
static int iObjAttr = sizeof(srawObjAttr);
static int irawFileSystem = sizeof(srawFileSystem);
static int irawCopyCache = sizeof(srawCopyCache);
static int irawAPIFile = sizeof(srawAPIFile);
                                           /* API file control block */
static int iOnlyQuery = 0;    /* =1: query only, no connection to DM */
static char cPrefix[MAX_FULL_FILE] = "";
static char cNodePrefix[MAX_NODE] = "";
static char cNodeMaster[MAX_NODE] = "";
static char cNodeMover[MAX_NODE] = "";
static int iPortMaster = PORT_MASTER;
static int iPortMover = PORT_MOVER;
static int iATLServer;    /* =1: aixtsm1(AIX), =2: gsitsma(Windows)
                             if < 0: corresponding test system       */

static int iStatusLoopMax = MAXLOOP_CACHE_FULL;
                               /* max iteration no. receiving status */
static int iSleepCacheFull = SLEEP_CACHE_FULL;
                        /* sleep time (s) waiting for cache cleaning */
static int iFileCount = 0;            /* currently open remote files */
static int iFileMax = 10;       /* API file control blocks available */
static RFILE *pAPIFile[10] =      /* ptrs to API file control blocks */
   {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

#ifdef __USE_LARGEFILE64

/********************************************************************
 * rfio_open64: open remote file in GSI mass storage
 *
 * created 22. 5.2007, Horst Goeringer
 ********************************************************************
 */

int rfio_open64(const char *pcFile,                     /* file name */
                int iFlags,
                int iMode)
{
   char cModule[32] = "rfio_open64";
   int iDebug = 0;
   int iFileId = -1;
   int ii;

   if (iDebug) fprintf(fLogClient,
      "\n-D- begin %s: file %s, flags %d, mode %d\n",
      cModule, pcFile, iFlags, iMode);
   
   ii = sizeof(long);
   if (ii < 8)
   {
      fprintf(fLogClient,
         "-E- 32 bit client: %s not supported\n", cModule);

      return -1;
   }

   iFileId = rfio_open(pcFile, iFlags, iMode);

   if (iDebug) fprintf(fLogClient,
      "-D- end %s: iFileId(rfio_open) = %d\n\n", cModule, iFileId);

   return iFileId;
   
} /* rfio_open64 */

#endif /* __USE_LARGEFILE64 */

/********************************************************************
 * rfio_open: open connection to gStore server and file
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_open(const char *pcFile,                       /* file name */
              int iFlags,
              int iOpenMode)
{
   char cModule[32] = "rfio_open";
   int iDebug = 0;
   int iFileId = -1;
   int iCopyMode = 0;      /* ignore parameters for data stream copy */
   char *pcCopyPath = NULL;
   int iCopyFraction = 0;
   int iMaxFile = 0;
   int iPathConvention = 0;

   if (iDebug)
      printf("\n-D- begin %s: calling rfio_open_gsidaq\n", cModule);

   iFileId = rfio_open_gsidaq(pcFile, iFlags, iOpenMode,
            iCopyMode,
            pcCopyPath,
            iCopyFraction,
            iMaxFile,
            iPathConvention);

   if (iDebug) printf(
      "-D- end %s, iFileId(rfio_open_gsidaq) = %d\n\n",
      cModule, iFileId);

   return iFileId;

} /* rfio_open */

/********************************************************************
 * rfio_open_gsidaq: open connection to gStore server and file and
 *     prepare copy to lustre or read cache before migration to tape
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_open_gsidaq(
        const char *pcFile,      /* fully qualified gStore file name */
        int iFlags,
        int iOpenMode,
        int iCopyMode,
               /* = 0: standard RFIO, ignore following arguments
                  = 1: copy to pcCopyPath after file written to WC
                       (for high data rates,
                        don't interfere writing to cache)
                  = 2: for lustre only:
                       write each data buffer in parallel to
                       write cache and to pcCopyPath
                       (for low data rates, anyhow first buffers
                        quickly available in lustre)
                */
        char *pcCopyPath,
               /* destination where to be copied
                  = "/lustre..."  => fully qualified path name
                    if not existing: will be created according
                       to user conventions:
                       ".../d" => Hades, create .../dyydddhhmm
                  = "RC" => read cache (not implemented)
                */
        int iCopyFraction,
               /* = i>0: copy each ith file to pcCopyPath
                */
        int iMaxFile,
               /* for dataFS only:
                  = 0: no file limit
                  > 0: max no. of files to be written to directory
                       files already existing are ignored
                       if iMaxFile reached, new dir will be created
                          (same level as previous one)
                */
        int iPathConvention)
               /* rules for creation of initial/new path
                   = 0: default convention
                     initially specified .../xxx => .../xxx
                     last .../xxx => create ...xxx1
                     last .../xxxi => create ...xxxj j=i+1
                   = 1: Hades convention: path name contains time stamp
                     initially specified .../xxx => .../xxxyydddhhmm
                     new => .../xxxyydddhhmm
                */
{
   char cModule[32] = "rfio_open_gsidaq";
   int iDebug = 0;
   int iRC;
   int iMaxConnect = 0;            /* try connection to servers once */
   int ii1, ii2;

#ifndef Lynx
   char cstr[16];
   time_t tstr;
   time_t tcur;
   struct tm *ptloc;
   pid_t pstr;
#endif

   unsigned long *plFileSizeC;          /* ptr to file size in sComm */
   int iObjInit = 1;   /* default: gStore object will be initialized */
   int iMassSto = -1;
         /* >0: connect to gStore
                =2: from DAQ to DAQPool (prefix contains rfiodaq:)
                =1: to other pools (RC or ArchivePool/WC)
            =0: connect to RFIO read/write server */
   int iStage = 0;                        /* = 1: file in stage pool */
   int iCache = 0;                       /* = 1: file in write cache */
   int iStatus = 0;
   int iStatusLen = 0;
   int iIdent;
   int iBuf, iBufl;
   int iDataMoverSelect = 0;

   int iStatusOkay = 0;
   int iStatusLoop = 0;    /* current iteration no. receiving status */

   int iPoolId = 0;
        /* for cmd buffer read  action: =2 (StagePool)
           for cmd buffer write action: =3: ArchivePool  =4: DaqPool */
   int iAction = 0;                           /* =1: read, =2: write */
   int iError = 0;
   int iSendEOS = 0;
     /* =1: send EOS to connected servers (master and/or data mover) */

   int iFileId = -1;        /* file identifier for server connection */
   char cServer[24] = "";
   int iSockMaster = -1;          /* socket for connection to master */
   int iSockMover = -1;            /* socket for connection to mover */

   int iSeekMode = -1;                           /* default: no seek */
   int iOffset = 0;                              /* default: no seek */
   int iBufsizeAct = 0;
   int iBufnoServ = 0;   /* no. of buffers transfered from/to server */
   int iDataServ = 0;      /* no. of bytes transfered from/to server */

   int iMapFound = 0;             /* =1: specified file already open */
   int ii;
   char *pcc, *pcc1;
   char cTemp[STATUS_LEN] = "";
   char cMsg[STATUS_LEN] = "                                                                                                                                                                                                         ";
   char cNamefs[MAX_OBJ_FS] = "", *pcNamefs;       /* filespace name */
   char cNamehl[MAX_OBJ_HL] = "", *pcNamehl;      /* high level name */
   char cNamell[MAX_OBJ_LL] = "", *pcNamell;      /* low  level name */
   char cObject[MAX_FULL_FILE] = "";

   char cOwner[DSM_MAX_OWNER_LENGTH] = "";
   char cliNode[MAX_NODE] = "";
   char cAccess[16] = "";

   struct tm buf_time;

   srawDataMover sDataMoverSelect;
                      /* buffer with data mover attrs sent by server */
   srawDataMoverAttr *pDataMoverSelect;       /* selected data mover */

   srawStatus sStatus;
   srawAPIFile *pcurAPIFile;
   srawComm *pCommAPI;  /* communication buffer in API control block */
   srawComm *pCommServ;            /* copy for servers in net format */
   srawObjAttr *pQueryAPI;                      /* query information */
   srawCopyCache *pCopyCacheServ;
                          /* infos in net format for copy of WC data */

   srawFileSystem sFileSystemServ;
       /* if file in ArchivePool: FS infos for servers in net format */

   void *pBufQuery;
   srawQueryResult sQuery;
   srawObjAttr *pQAttr = NULL;

   if (fLogFile == NULL)
      fLogFile = stdout;
   if (fLogClient == NULL)
      fLogClient = stdout;

   pDataMoverSelect = &(sDataMoverSelect.sDataMoverAttr);
   pcNamefs = cNamefs;
   pcNamehl = cNamehl;
   pcNamell = cNamell;

   if (iFlags == O_RDONLY)
   {
      iAction = 1;
      strcpy(cAccess, "read");
   }
   else
   {
      iAction = 2;
      strcpy(cAccess, "write");
   }

   ii = sizeof(long);
   if (ii == 8)
      iOS64 = 1;
   else if (ii != 4) printf(
      "-W- unexpected size of long integer: %d byte\n", ii);

   if (iDebug)
   {
      ii *= 8;                              /* size of 'long' in bit */
      printf("\n-D- begin %s (%d bit OS)", cModule, ii);
      if (iFileCount)
         printf(", %d remote files currently open", iFileCount);
      printf("\n");
   }

   ii = 0;
   if (iFileCount > 0)
      while (ii < iFileMax)
   {
      if (pAPIFile[ii] != NULL)
      {
         pCommAPI = &(pAPIFile[ii]->sCommAPI);
         strcpy(cObject, pCommAPI->cNamefs);
         strcat(cObject, pCommAPI->cNamehl);
         strcat(cObject, pCommAPI->cNamell);

         if (iDebug)
            printf("    %d: %s already open\n", ii, cObject);

         pcc = strstr(pcFile, cObject);
         if (pcc != NULL)
         {
            iMapFound = 1;
            if (iDebug)
               printf("    %d: %s maps\n", ii, cObject);
            break;
         }
      }
      else if (iDebug)
         printf("    %d: no (more) open file found\n", ii);

      ii++;
   }

   if (iMapFound)
   {
      fprintf(fLogClient,
         "-E- %s: requested file %s is already open\n",
         cModule, pcFile);

      iError = -1;
      goto gError;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient,
         "-E- %s: max %d remote files may be open simultaneously\n",
         cModule, iFileMax);

      iError = -1;
      goto gError;
   }

   pAPIFile[ii] = (srawAPIFile *) malloc((unsigned) irawAPIFile);
   if (pAPIFile[ii] == NULL)
   {
      fprintf(fLogClient,
         "-E- %s: allocating file control block no. %d (%d byte)\n",
         cModule, ii, irawAPIFile);
      perror("    ");

      iError = -1;
      goto gError;
   }

   iFileId = ii;
   iFileCount++;
   if (iDebug) printf(
      "    file control block no. %d allocated (%d byte)\n",
      ii, irawAPIFile);

   pcurAPIFile = pAPIFile[ii];
   if (iOpenMode < 0)                                  /* query only */
   {
      iOpenMode = 0;
      iOnlyQuery = 1;
      pcurAPIFile->iOpMode = 2;

      if (iDebug)
         printf("    query only, iOpenMode %d reset to 0\n", iOpenMode);
   }
   else
      pcurAPIFile->iOpMode = 0;

   pcurAPIFile->iFileId = iFileId;
   pcurAPIFile->iFlags = iFlags;
   pcurAPIFile->iMode = iOpenMode;
   pcurAPIFile->iSockMaster = -1;
   pcurAPIFile->iSockMover = -1;
   pcurAPIFile->iSeekMode = iSeekMode;
   pcurAPIFile->iOffset = iOffset;
   pcurAPIFile->iBufsizeAct = iBufsizeAct;
   pcurAPIFile->iBufnoServ = iBufnoServ;
   pcurAPIFile->iDataServ = iDataServ;
   pcurAPIFile->iAction = iAction;

   pCommAPI = &(pcurAPIFile->sCommAPI);
   pCommAPI->iIdent = IDENT_COMM;
   pCommAPI->iCommLen = irawComm - HEAD_LEN;
   if (iOS64)
      pCommAPI->iClient32 = 0;
   else
      pCommAPI->iClient32 = 1;

   pQueryAPI = &(pcurAPIFile->sQueryAPI);
   pCommServ = &(pcurAPIFile->sCommServ);
   pCopyCacheServ = &(pcurAPIFile->sCopyCacheServ);

   /**************** analyze remote file specification **************/

   if ( (strchr(pcFile, *pcStar) != NULL) ||
        (strchr(pcFile, *pcQM) != NULL) ||
        (strchr(pcFile, *pcPerc) != NULL) )
   {
      fprintf(fLogClient,
         "-E- %s: files with wildcard characters ('%s') not allowed using RFIO\n",
         cModule, pcFile);

      iError = -1;
      goto gError;
   }

   /* handle prefixes to object name */
   pcc = (char *) strchr(pcFile, *pcDevDelim);
   if (pcc == NULL)
   {
      fprintf(fLogClient, 
        "-E- %s(1): invalid remote file name '%s': node name missing\n",
        cModule, pcFile);

      iError = -1;
      goto gError;
   }

   /* convert prefix and/or node to lower case (in cTemp) */
   strcpy(cTemp, pcFile);
   pcc = (char *) strrchr(cTemp, *pcDevDelim);
   pcc++;
   strncpy(pcc, "\0", 1);       /* terminates after prefix: or node: */
   pcc1 = cTemp;
   strcpy(cPrefix, pcc1);     /* keep for case of name modifications */
   while (*pcc1 != '\0')
   {
      *pcc1 = tolower(*pcc1);
      pcc1++;
   }

   /* cTemp contains now prefix: and/or node: in small letters */
   if (iDebug)
      printf("    URL in small letters: '%s'\n", cTemp);

   /* remove trailing '.gsi.de', if specified */
   pcc = strstr(cTemp, ".gsi.de");
   if (pcc)
   {
      strncpy(pcc++, ":", 1);
      strncpy(pcc, "\0", 1);
      if (iDebug)
         printf("    remove trailing '.gsi.de': %s\n", cTemp);
   }

   /* rfiocopy: or rfiodaq: or rfio: */
   if (strncmp(cTemp, "rfio", 4) == 0)
   {
      pcc = (char *) strchr(cTemp, *pcDevDelim);
      pcc++;                                /* skip device delimiter */
      pcc1 = (char *) strrchr(pcc, *pcDevDelim);
      if (pcc1 == NULL)
      {
        fprintf(fLogClient, 
           "-E- %s(2): invalid prefix '%s' of remote file name: node name missing\n",
           cModule, pcFile);

        iError = -1;
        goto gError;
      }
      strncpy(pcc1, "\0", 1);          /* terminates after node name */

      /* now pcc points to node name */
      if (strlen(pcc) == 0)
      {
        fprintf(fLogClient, 
           "-E- %s(3): invalid remote file name '%s': node name missing\n",
           cModule, pcFile);

        iError = -1;
        goto gError;
      }

      strcpy(cNodePrefix, pcc);
      if (iDebug)
         printf("    node name after prefix 'rfio': '%s'\n", cNodePrefix);

      if (strncmp(cTemp, "rfiodaq:", 8) == 0)
      {
         if ( (iAction == 1) && (iOnlyQuery == 0) )
         {
            fprintf(fLogClient, "-E- %s: reading from DaqPool not allowed\n",
               cModule);
            fprintf(fLogClient, "    correct object specification:\n");
            fprintf(fLogClient, "    [rfio:]node:/archive/path/file\n");

           iError = -1;
           goto gError;
         }

         iMassSto = 2;                                    /* DAQPool */
         strcpy(cServer, "gStore entry server");
         iPoolId = 4;
      }
      else if (strncmp(cTemp, "rfio:", 5) == 0)
      {
         iMassSto = 1;                                /* ArchivePool */
         strcpy(cServer, "gStore entry server");
         if (iAction == 1)
         {
            if (iOnlyQuery == 0)
               iPoolId = 2;                             /* StagePool */
         }
         else if (iAction == 2)
            iPoolId = 3;                              /* ArchivePool */
      }
      else if (strncmp(cTemp, "rfiocopy:", 9) == 0)
      {
         iMassSto = 0;
         strcpy(cServer, "RFIO server");
         iPoolId = 0;                              /* not applicable */
         iPortMaster = PORT_RFIO_SERV;
         iPortMover = PORT_RFIO_SERV;
      }
      else
      {
         fprintf(fLogClient, 
            "-E- %s: invalid remote file name '%s': unknown prefix\n",
            cModule, pcFile);

         iError = -1;
         goto gError;
      }
   } /* (strncmp(cTemp, "rfio", 4) == 0) */
   else
   {
      /* node:/object */
      pcc = (char *) strchr(cTemp, *pcDevDelim);
      pcc1 = (char *) strrchr(pcc, *pcDevDelim);
      if (pcc1 != pcc)             /* more than one device delimiter */
      {
         fprintf(fLogClient,
            "-E- %s: invalid remote file name '%s': invalid prefix\n",
            cModule, pcFile);

         iError = -1;
         goto gError;
      }

      if (strlen(pcc) == 0)
      {
         fprintf(fLogClient,
            "-E- %s(4): invalid remote file name '%s': node name missing\n",
            cModule, pcFile);

         iError = -1;
         goto gError;
      }

      iMassSto = 1;
      strcpy(cServer, "gStore entry server");
      if (iAction == 1)
      {
         if (iOnlyQuery == 0)
            iPoolId = 2;                                /* StagePool */
      }
      else if (iAction == 2)
         iPoolId = 3;                                 /* ArchivePool */

      strncpy(pcc, "\0", 1);           /* terminates after node name */
      strcpy(cNodePrefix, cTemp);
      if (iDebug)
         printf("    node name: '%s'\n", cNodePrefix);
   }

   if ( (strcmp(cNodePrefix, "adsmexp") == 0) ||
        (strcmp(cNodePrefix, "gsitsma") == 0) )
   {
      fprintf(fLogClient, "-E- %s: gStore server %s no longer supported\n",
         cModule, cNodePrefix);

      iError = -1;
      goto gError;
   }

   pcurAPIFile->iMassSto = iMassSto;
   if (iDebug)
   {
      if ( (iAction == 2) ||
           ((iAction == 1) && (iOnlyQuery == 0)) )
         printf("    %s, request poolId %d\n", cServer, iPoolId);
      else
         printf("    %s\n", cServer);
   }

   /******************** now handle object name **********************/

   pcc = (char *) strrchr(pcFile, *pcDevDelim);
   pcc++;

   if ( (strlen(pcc) == 0) || (strcmp(pcc, " ") == 0) )
   {
      if (iAction == 1)
      {
         fprintf(fLogClient,
            "-E- %s: missing remote object for read access\n", cModule);

         iError = -1;
         goto gError;
      }

      pcurAPIFile->iOpMode = 1;
      iObjInit = 0;
          /* no open of remote file, requires following rfio_newfile */
      if (iDebug)
      {
         if (iMassSto > 0)
            printf("    remote object will be initialized later\n");
         else
            printf("    remote file will be initialized later\n");
      }

      /* "" or "\0" are corrupted by TCP transfer, must be
         initialized again in server */
      strcpy(pcNamefs, "");
      strcpy(pcNamehl, "");
      strcpy(pcNamell, "");
      strcpy(pCommAPI->cNamefs, "");
      strcpy(pCommAPI->cNamehl, "");
      strcpy(pCommAPI->cNamell, "");
   }
   else
   {
      iObjInit = 1;
      strcpy(cTemp, pcc);             /* cTemp contains now obj name */
      pcc = (char *) strchr(cTemp, *pcObjDelim);
      if (pcc == NULL)
      {
         fprintf(fLogClient,
            "-E- %s: invalid remote file name %s\n", cModule, cTemp);

         iError = -1;
         goto gError;
      }

      pcc++;
      pcc1 = (char *) strchr(pcc, *pcObjDelim);

      if (iMassSto > 0)
      {
         if (iDebug)
            printf("    remote object %s\n", cTemp);

         if ( (pcc == NULL) || (pcc1 == NULL) )
         {
            fprintf(fLogClient,
               "-E- %s: invalid remote object name '%s': archive name missing\n",
               cModule, pcFile);

            iError = -1;
            goto gError;
         }

         pcc = pcc1;
         strncpy(pcc1, "\0", 1);          /* terminates archive name */
         if (strlen(cTemp) > MAX_OBJ_FS-1)
         {
            ii = MAX_OBJ_FS-1;
            fprintf(fLogClient, "-E- %s: archive name '%s' too long (max %d)\n",
               cModule, cTemp, ii);

            iError = -1;
            goto gError;
         }

         if (strlen(cTemp) < 2)
         {
            fprintf(fLogClient, "-E- %s: invalid archive name '%s'\n",
               cModule, cTemp);

            iError = -1;
            goto gError;
         }

         strcpy(pcNamefs, cTemp);
         if (iDebug)
            printf("    archive name %s\n", pcNamefs);

         strncpy(pcc, pcObjDelim, 1);          /* recover obj delim */
         pcc++;
         pcc1 = (char *) strrchr(pcc, *pcObjDelim);
         if (pcc1 == NULL)
         {
            fprintf(fLogClient,
               "-E- %s: invalid remote object name '%s': path name missing\n",
               cModule, pcFile);

            iError = -1;
            goto gError;
         }

         if (strlen(pcc1) > MAX_OBJ_LL-1)
         {
            ii = MAX_OBJ_LL-1;
            fprintf(fLogClient, "-E- %s: file name '%s' too long (max %d)\n",
               cModule, pcc1, ii);

            iError = -1;
            goto gError;
         }

         if (strlen(pcc1) < 2)
         {
            fprintf(fLogClient, "-E- %s: invalid file name '%s'\n", cModule, pcc1);

            iError = -1;
            goto gError;
         }

         strcpy(pcNamell, pcc1);
         if (iDebug)
            printf("    file name %s\n", pcNamell);

         pcc--;
         strncpy(pcc1, "\0", 1);       /* terminates after path name */
         if (strlen(pcc) > MAX_OBJ_HL-1)
         {
            ii = MAX_OBJ_HL-1;
            fprintf(fLogClient, "-E- %s: path name '%s' too long (max %d)\n",
               cModule, pcc, ii);

            iError = -1;
            goto gError;
         }

         if (strlen(pcc) < 2)
         {
            fprintf(fLogClient, "-E- %s: invalid path name '%s'\n", cModule, pcc);

            iError = -1;
            goto gError;
         }

         strcpy(pcNamehl, pcc);
         if (iDebug)
            printf("    path name %s\n", pcNamehl);

         strcpy(pCommAPI->cNamefs, rawGetFSpName(pcNamefs));
         iRC = rawGetLLName(pcNamell, pcObjDelim, pCommAPI->cNamell);
         if (iRC)
         {
            fprintf(fLogClient, "-E- %s: invalid gStore file name: %s\n",
               cModule, pCommAPI->cNamell);

            iError = -1;
            goto gError;
         }

         strcpy(pCommAPI->cNamehl, rawGetHLName(pcNamehl));
         if (strcmp(pcNamehl, pCommAPI->cNamehl))
         {
            strcpy(pcNamehl, pCommAPI->cNamehl);
            printf("    gStore path name corrected to %s\n", pcNamehl);

            strcpy((char *) pcFile, cPrefix);
            strcat((char *) pcFile, pCommAPI->cNamefs);
            strcat((char *) pcFile, pCommAPI->cNamehl);
            strcat((char *) pcFile, pCommAPI->cNamell);
            if (iDebug) printf(
               "    new full file name: %s\n", pcFile);
         }
      } /* (iMassSto > 0) */
      else
      {
         if (iDebug)
            printf("    remote file %s\n", cTemp);

         strcpy(pcNamefs, "");
                          /* start with path, fs name has no meaning */

         pcc1 = (char *) strrchr(cTemp, *pcObjDelim);
         if (pcc1 == NULL)
         {
            fprintf(fLogClient, "-E- %s: invalid remote file name '%s'\n",
               cModule, cTemp);

            iError = -1;
            goto gError;
         }

         if (strlen(pcc1) > MAX_OBJ_LL-1)
         {
            ii = MAX_OBJ_LL-1;
            fprintf(fLogClient, "-E- %s: file name '%s' too long (max %d)\n",
               cModule, pcc1, ii);

            iError = -1;
            goto gError;
         }

         if (strlen(pcc1) < 2)
         {
            fprintf(fLogClient, "-E- %s: invalid file name '%s'\n", cModule, pcc1);

            iError = -1;
            goto gError;
         }

         strcpy(pcNamell, pcc1);
         if (iDebug)
            printf("    file name %s\n", pcNamell);

         strncpy(pcc1, "\0", 1);       /* terminates after path name */
         if (strlen(cTemp) > MAX_OBJ_HL-1)
         {
            ii = MAX_OBJ_HL-1;
            fprintf(fLogClient, "-E- %s: path name '%s' too long (max %d)\n",
               cModule, cTemp, ii);

            iError = -1;
            goto gError;
         }

         if (strlen(cTemp) < 2)
         {
            fprintf(fLogClient, "-E- %s: invalid path name '%s'\n",
               cModule, cTemp);

            iError = -1;
            goto gError;
         }

         strcpy(pcNamehl, cTemp);
         if (iDebug)
            printf("    path name %s\n", pcNamehl);

         strcpy(pCommAPI->cNamefs, "\0");
         strcpy(pCommAPI->cNamehl, pcNamehl);
         strcpy(pCommAPI->cNamell, pcNamell);

      } /* (iMassSto == 0) */
   } /* (iObjInit) */
 
   /*********** check if copy from WC before tape migration *********/

   if (iCopyMode)
   {
      if (iAction != 2)
      {
         fprintf(fLogClient,
            "-E- copy from write cache only for write actions\n");

         iError = -1;
         goto gError;
      }

      if (iMassSto != 2)
      {
         fprintf(fLogClient,
            "-E- copy from write cache to lustre only when writing to DAQPool\n");

         iError = -1;
         goto gError;
      }

      if (iObjInit)
      {
         fprintf(fLogClient,
            "-E- copy from write cache only supported with rfio_[f]newfile\n");

         iError = -1;
         goto gError;
      }

      if ( (iCopyMode < 0) || (iCopyMode > 2) )
      {
         fprintf(fLogClient, "-E- invalid copy mode: %d\n", iCopyMode);

         iError = -1;
         goto gError;
      }

      if ( (strlen(pcCopyPath) == 0) || (pcCopyPath == NULL) )
      {
         fprintf(fLogClient, "-E- copy path missing\n");

         iError = -1;
         goto gError;
      }

      if (iCopyFraction < 0)
      {
         fprintf(fLogClient,
            "-E- invalid fraction of files to copied: %d (must be > 0)\n",
            iCopyFraction);

         iError = -1;
         goto gError;
      }

      if (iCopyFraction == 0)
      {
         fprintf(fLogClient,
            "-W- fraction of files to copied: 0 specified, assume 1 (copy all)\n");

         iCopyFraction = 1;
      }

      strcpy(cTemp, pcCopyPath);
      pcc = cTemp;

      /* for check, convert to lower case */
      while (*pcc != '\0')
      {
         *pcc = tolower(*pcc);
         pcc++; /* gcc 3.3.5: must be two steps, else string corrupted */
      }
      if (strcmp(cTemp, "rc") == 0)
      {
         fprintf(fLogClient,
            "-E- copy to read cache not yet implemented\n");

         iError = -1;
         goto gError;
      }

      strcpy(cTemp, pcCopyPath);           /* use mixed case letters */

      /* check for trailing '/' */
      ii1 = 1;
      ii2 = 0;                          /* count no. of trailing '/' */
      pcc = cTemp;
      while (ii1)
      {
         pcc1 = strrchr(cTemp, '/');
         ii = pcc1 - pcc + 1;
         if (ii == strlen(pcc))
         {
            ii2++;
            strncpy(pcc1, "\0", 1);
         }
         else 
            ii1 = 0;
      }

      if (ii2)
      {
         strcpy(pcCopyPath, cTemp);
         fprintf(fLogClient,
            "    %d trailing '/' ignored, use copy destination '%s'\n",
            ii2, pcCopyPath);
      }

      /* check for valid lustre FS, at least one subdir */
      if ( (strncmp(pcCopyPath, cDataFSHigh1, strlen(cDataFSHigh1))) &&
           (strncmp(pcCopyPath, cDataFSHigh2, strlen(cDataFSHigh2))) )
      {
         fprintf(fLogClient,
            "-E- invalid copy destination %s\n", pcCopyPath);

         iError = -1;
         goto gError;
      }

      if (iMaxFile < 0)
      {
         fprintf(fLogClient,
            "-E- invalid max file no. for copy destination: %d\n",
            iMaxFile);

         iError = -1;
         goto gError;
      }

      if ( (iPathConvention < 0) || (iPathConvention > 1) )
      {
         fprintf(fLogClient,
            "-E- invalid convention no. for creation of new copy destination path: %d\n",
            iPathConvention);

         iError = -1;
         goto gError;
      }

      if (iCopyMode == 1)
      {
         if (iCopyFraction == 1) fprintf(fLogClient,
            "-I- copy each file to %s", pcCopyPath);
         else fprintf(fLogClient,
            "-I- copy each %d. file to %s", iCopyFraction, pcCopyPath);
         if (iPathConvention == 1)
            fprintf(fLogClient, "*");
         fprintf(fLogClient, " before tape migration\n");
         fprintf(fLogClient, "    check success as asynchroneous copy process\n");
      }
      else
      {
         if (iCopyFraction == 1) fprintf(fLogClient,
            "-I- write data buffers of each file to gStore write cache and to %s",
            pcCopyPath);
         else fprintf(fLogClient,
            "-I- for each %d. file write data buffers additionally to %s",
            iCopyFraction, pcCopyPath);
         if (iPathConvention == 1)
            fprintf(fLogClient, "*\n");
         else
            fprintf(fLogClient, "\n");
      }
      if ( (iMaxFile) && (strcmp("rc", pcCopyPath)) ) fprintf(fLogClient,
         "    if %d files are written, create new directory (convention no. %d)\n",
         iMaxFile, iPathConvention);

      /* store parameters */
      pCopyCacheServ->iIdent = htonl( (unsigned int) IDENT_COPY_CACHE);
      pCopyCacheServ->iCopyMode = htonl(iCopyMode);
      pCopyCacheServ->iCopyLen = htonl(0);    /* indicates that copy 
                      buffer not yet sent to DM: set in rfio_newfile */
      strcpy(pCopyCacheServ->cCopyPath, pcCopyPath);
      pCopyCacheServ->iCopyFraction = htonl(iCopyFraction);
      pCopyCacheServ->iMaxFile = htonl(iMaxFile);
      pCopyCacheServ->iPathConvention = htonl(iPathConvention);

      if (iDebug) printf(
         "    parameters for write cache copy stored\n");

   } /* (iCopyMode) */

   if (iMassSto > 0)
   {
      iPortMaster = PORT_MASTER;
      if (iCopyMode)
         iPortMover = PORT_MOVER_DATAFS;
      else
         iPortMover = PORT_MOVER;

      pcc = cNodePrefix + strlen(cNodePrefix) - 2;   /* last 2 chars */
      if (strcmp(pcc, "_d") == 0)
      {
         strncpy(pcc, "\0", 1);              /* terminates node name */
         if (iDebug)
            printf("    switch to test system\n");

         iPortMaster -= 10;
         iPortMover -= 10;
      }

      if ( (strcmp(cNodePrefix, "gstore") == 0) ||     /* in root, C */
           (strcmp(cNodePrefix, "//lxgstore") == 0) ||    /* in root */
           (strcmp(cNodePrefix, "///lxgstore") == 0) ||   /* in root */
           (strcmp(cNodePrefix, "///gstore") == 0) ||     /* in root */
           (strcmp(cNodePrefix, "//gstore") == 0) )       /* in root */
         strcpy(cNodeMaster, cNodeMaster0);
      else
         strcpy(cNodeMaster, cNodePrefix);

      if (iDebug) printf(
         "    gStore entry server %s (node in URL %s)\n",
         cNodeMaster, cNodePrefix);
      fflush(stdout);
   }
   else
      strcpy(cNodeMaster, cNodePrefix);

   strcpy(pCommAPI->cNodeCacheMgr, cNodeMaster);

   pCommAPI->iSynchId = 0;   /* =1: keep DM session after file close */
   pCommAPI->iExecStatus = 0;                              /* unused */
   pCommAPI->iWaitTime = 0;                                /* unused */
   strcpy(pCommAPI->cTapeLib, "0");          /* default tape library */

   if ( iAction == 1)
   {
      if (iOnlyQuery)
         pCommAPI->iAction = QUERY;
      else
         pCommAPI->iAction = QUERY_RETRIEVE_RECORD;/* at first query */
   }
   else
   {
      if (iMassSto)
         pCommAPI->iAction = QUERY_ARCHIVE_RECORD; /* at first query */
      else
         pCommAPI->iAction = ARCHIVE_RECORD;/* RFIO server: no query */
   }

   if (iDebug)
   {
      if (iMassSto == 0)
      {
         if (iObjInit) printf(
            "    %s node %s:%d, file %s%s",
            cServer, cNodeMaster, iPortMaster, pcNamehl, pcNamell);
         else printf(
            "    %s node %s:%d", cServer, cNodeMaster, iPortMaster);
      }
      else
      {
         if (iObjInit) printf(
            "    %s %s:%d, TSM object %s%s%s",
            cServer, cNodeMaster, iPortMaster,
            pcNamefs, pcNamehl, pcNamell);
         else printf(
            "    %s %s:%d", cServer, cNodeMaster, iPortMaster);
      }
      if (iPoolId)
      {
         if (iPoolId == 2)
            printf(", request StagePool\n");
         else if (iPoolId == 3)
            printf(", request ArchivePool\n");
         else if (iPoolId == 4)
            printf(", request DaqPool\n");
      }
      else
         printf("\n");

      if (iObjInit) printf(
         "    FS name '%s', HL name '%s', LL name '%s', action %d\n",
         pCommAPI->cNamefs, pCommAPI->cNamehl, pCommAPI->cNamell,
         pCommAPI->iAction);
      else printf(
         "    action %d, object follows later\n", pCommAPI->iAction);

   } /* (iDebug) */

   /******************** get client information *********************/

   strcpy(cOwner, rawGetUserid());
   iRC = gethostname(cliNode, MAX_NODE);
   if (iRC)
   {
      fprintf(fLogClient, "-E- %s: getting client host name: %s\n",
         cModule, strerror(iRC));

      iError = -1;
      goto gError;
   }

   if (iDebug) printf(
      "    user %s on node %s, platform %s\n",
      cOwner, cliNode, cOS);                   /* cOS from rawcli.h */

#ifndef Lynx
   iLogFile = 0;                         /* client output to stdout */
   if ( (iMassSto) &&           /* not for rfiocopy: has server log */
        (fLogClient == stdout) &&                /* not yet to file */
        (iLogFile) )                                  /* local flag */
   {
      strcpy(cLogClient, "./RFIO");
      strcat(cLogClient, cOwner);
      tstr = time(NULL);
      sprintf(cstr, ".t%d", (int) tstr);
      strcat(cLogClient, cstr);
      pstr = getpid();
      sprintf(cstr, ".p%d", pstr);
      strcat(cLogClient, cstr);
      strcat(cLogClient, ".log");

      ii1 = strlen(cLogClient);
      ii2 = MAX_LOG_FILE;
      if (ii1 >= ii2)
         printf("-E- %s: log file name %s too long (%d, max %d)\n",
            cModule, cLogClient, ii1, --ii2);

         fLogClient = fopen(cLogClient, "w");
         if (fLogClient == NULL)
         {
            printf("-W- can not open log file %s, write to stdout\n",
               cLogClient);
            perror("    ");
            fLogClient = stdout;
         }
   
      printf("-I- log file %s\n", cLogClient);
      fprintf(fLogClient, "*** log file: %s, client node: %s\n",
         cLogClient, cliNode);

      /* get current date and time */
      tcur = time(NULL);
      ptloc = localtime_t(&tcur, &buf_time);
      (ptloc->tm_mon)++;              /* else starts with zero */
      ptloc->tm_year += 1900;

      fprintf(fLogClient,
         "    current date %d/%.2d/%.2d, time %.2d:%.2d:%.2d\n",
         ptloc->tm_year,
         ptloc->tm_mon,
         ptloc->tm_mday,
         ptloc->tm_hour,
         ptloc->tm_min,
         ptloc->tm_sec);
      fflush(fLogClient);

   } /* (iMassSto) */
#endif

   strcpy(pCommAPI->cApplType, cApplType);
   strcpy(pCommAPI->cOwner,  cOwner);
   strcpy(pCommAPI->cliNode, cliNode);
   strcpy(pCommAPI->cOS, cOS);
   strcpy(pCommAPI->cDataFS, "");     /* not supported in API client */
   pCommAPI->iDataFS = 0;

   if (iAction == 2)
   {
      pCommAPI->iPoolIdWC = iPoolId;
      pCommAPI->iPoolIdRC = 0;
      strcpy(pCommAPI->cNodeRC, "");
      pCommAPI->iStageFSid = 0;

      if (iPoolId == 4)
         pCommAPI->iArchDev = ARCH_DAQ_DISK;
      else
         pCommAPI->iArchDev = ARCH_DISK;
   }
   else
   {
      pCommAPI->iPoolIdRC = iPoolId;
      pCommAPI->iPoolIdWC = 0;
      pCommAPI->iArchDev = RETR_STAGE_TEMP;
   }

   if (iDebug)
   {
      if (iAction == 2)
         printf("    request WC poolId %d, device %d\n",
            pCommAPI->iPoolIdWC, pCommAPI->iArchDev);
      else if (iOnlyQuery == 0)
         printf("    request RC poolId %d, device %d\n",
            pCommAPI->iPoolIdRC, pCommAPI->iArchDev);
   }

   pCommServ->iIdent = htonl(pCommAPI->iIdent);
   pCommServ->iAction = htonl(pCommAPI->iAction);
   pCommServ->iCommLen = htonl(pCommAPI->iCommLen);
   pCommServ->iBufsizeFile = htonl(0);
   pCommServ->iSynchId = htonl(pCommAPI->iSynchId);
   pCommServ->iExecStatus = htonl(pCommAPI->iExecStatus);
   pCommServ->iWaitTime = htonl(pCommAPI->iWaitTime);
   strcpy(pCommServ->cNamefs, pCommAPI->cNamefs);
   strcpy(pCommServ->cNamehl, pCommAPI->cNamehl);
   strcpy(pCommServ->cNamell, pCommAPI->cNamell);
   strcpy(pCommServ->cOwner,  pCommAPI->cOwner);
   strcpy(pCommServ->cOS, pCommAPI->cOS);
   strcpy(pCommServ->cApplType, pCommAPI->cApplType);
   strcpy(pCommServ->cliNode, pCommAPI->cliNode);
   strcpy(pCommServ->cTapeLib, pCommAPI->cTapeLib);
   strcpy(pCommServ->cDataFS, pCommAPI->cDataFS);
   strcpy(pCommServ->cNodeCacheMgr, pCommAPI->cNodeCacheMgr);
   pCommServ->iDataFS = htonl(pCommAPI->iDataFS);
   pCommServ->iArchDev = htonl(pCommAPI->iArchDev);
   pCommServ->iPoolIdWC = htonl(pCommAPI->iPoolIdWC);
   pCommServ->iPoolIdRC = htonl(pCommAPI->iPoolIdRC);
   pCommServ->iStageFSid = htonl(pCommAPI->iStageFSid);
   pCommServ->iPoolIdRC = htonl(pCommAPI->iPoolIdRC);
   pCommServ->iClient32 = htonl(pCommAPI->iClient32);

   /******************* connect to entry server **********************/

   if (iDebug) printf(
      "    connecting to %s %s:%d\n",
      cServer, cNodeMaster, iPortMaster);

   iRC = rconnect(cNodeMaster, iPortMaster, &iMaxConnect, &iSockMaster);
   if (iRC)
   {
      fprintf(fLogClient, "-E- %s: cannot connect to %s %s:%d\n",
         cModule, cServer, cNodeMaster, iPortMaster);
      iSendEOS = 0;
      iError = -1;
      goto gClose;
   }

   strcpy(pcurAPIFile->cNodeMaster, cNodeMaster);
   pcurAPIFile->iSockMaster = iSockMaster;

   if (iDebug)
   {
      printf("-I- successfully connected to %s %s:%d",
             cServer, cNodeMaster, iPortMaster);
      if (iMaxConnect)
         printf(" after %ds", iMaxConnect);
      printf(" (socket %d)\n", iSockMaster);
   }

   if (iMassSto == 0)
   {
      pCommServ->iFileType = htonl(STREAM);
      pCommServ->iBufsizeFile = htonl(0);
      pCommServ->iFileSize = htonl(0);
      pCommServ->iFileSize2 = htonl(0);
      pCommServ->iStageFSid = htonl(0);

      /* provide query infos to other API procedures */
      pCommAPI->iFileType = ntohl(pCommServ->iFileType);
      pCommAPI->iBufsizeFile = ntohl(pCommServ->iBufsizeFile);
      pCommAPI->iFileSize = ntohl(pCommServ->iFileSize);
      pCommAPI->iFileSize2 = ntohl(pCommServ->iFileSize2);
      pCommAPI->iStageFSid = ntohl(pCommServ->iStageFSid);

      /* rfiocopy: master and mover identical */
      strcpy(cNodeMover, cNodeMaster);
      strcpy(pcurAPIFile->cNodeMover, cNodeMaster);
      iSockMover = iSockMaster;
      pcurAPIFile->iSockMover = iSockMaster;

      iSockMaster = -1;
      pcurAPIFile->iSockMaster = -1;

   } /* (iMassSto == 0) */
   else
   {
      if (iAction == 2)
         iATLServer = ATLSERVER_ARCHIVE;
      else
         iATLServer = 0;                /* query in both ATL servers */

      pCommAPI->iATLServer = iATLServer;
      pCommServ->iATLServer = htonl(iATLServer);

      if (iObjInit)
      {
         /********************* perform query ************************/

         pCommAPI->iSynchId = 0;                            /* reset */
         pCommServ->iSynchId = htonl(pCommAPI->iSynchId);

         pBufQuery = &sQuery;
         pQAttr = &(sQuery.objAttr);

         if (iDebug)
         {
            printf("    query file %s in gStore\n", pcFile);
            ii = 1;
         }
         else
            ii = 0;

         iRC = rawQueryFile(iSockMaster, ii, pCommServ, &pBufQuery);

         if (iAction != 2)                                  /* reset */
         {
            iATLServer = ntohl(pQAttr->iATLServer);
            pCommAPI->iATLServer = iATLServer;
            pCommServ->iATLServer = htonl(iATLServer);
         }

         if (iRC < 0)
         {
            if (iRC == -1001)
            {
               if (iATLServer == 0) sprintf(rfio_errmsg,
                  "-E- no %s access to archive %s in all ATL servers\n",
                  cAccess, pCommAPI->cNamefs);
               else sprintf(rfio_errmsg,
                  "-E- no %s access to archive %s in ATL server %d\n",
                  cAccess, pCommAPI->cNamefs, iATLServer);
            }
            else if (iRC == -1000) sprintf(rfio_errmsg,
               "-E- no archive %s in ATL server %d found\n",
               pCommAPI->cNamefs, iATLServer);
            else sprintf(rfio_errmsg,
                "-E- file %s: query in ATL server %d failed, rc = %d\n",
                pcFile, iATLServer, iRC);
            fprintf(fLogClient, "%s", rfio_errmsg);

            iSendEOS = 1;
            iError = -1;
            goto gClose;
         }

         if (iRC == 0)
         {
            if (iAction == 1)                                /* read */
            {
               if (iOnlyQuery)
                  sprintf(rfio_errmsg,
                     "    file %s not available in gStore\n", pcFile);
               else
                  sprintf(rfio_errmsg,
                     "-E- %s: file %s not available in gStore\n",
                     cModule, pcFile);
               fprintf(fLogClient, "%s", rfio_errmsg);

               iError = 1;
               iSendEOS = 1;
               goto gClose;
            }

            if (iDebug) printf(
               "    file %s not yet available in gStore\n", pcFile);
         }
         else
         {
            if ( (iAction == 1) &&                           /* read */
                 (iOnlyQuery == 0) )                     /* get data */
            {
               if ( (pQAttr->iFileSize2) && (iOS64 == 0) )
               {
                  sprintf(rfio_errmsg,
                     "-E- filesize of %s >= 4 GB, cannot be read with 32 bit client\n",
                     pcFile);
                  fprintf(fLogClient, "%s", rfio_errmsg);

                  iSendEOS = 1;
                  iError = 1;
                  goto gClose;
               }
            }
            else if (iAction == 2)                          /* write */
            {
               sprintf(rfio_errmsg,
                  "-E- file %s already available in gStore\n", pcFile);
               fprintf(fLogClient, "%s", rfio_errmsg);

               iSendEOS = 1;
               iError = 1;
               goto gClose;
            }

            memcpy(pQueryAPI, pQAttr, (unsigned) iObjAttr);
 
            if (iDebug)
            {
               printf("    file %s available in gStore (ATL server %d)\n",
                  pcFile, iATLServer);

               if (iRC > 1) printf(
                  "-W- %d versions of file %s exist\n", iRC, pcFile);
               ii = 1;
            }
            else
               ii = 0;

            rawQueryString(pQueryAPI, ii, sizeof(cMsg), cMsg);
            if ( (iDebug) || (iOnlyQuery) )
               printf("    %s", cMsg);

            iStage = ntohl(pQAttr->iFS);
            iCache = 0;                             /* reset/default */
            pCommAPI->iStageFSid = 0;
            pCommAPI->iFSidWC = 0;
            if ( (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_STAGE) ||
                 (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_INCOMPLETE) ||
                 (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_LOCKED) )
            {
               strcpy(cNodeMover, pQAttr->cNode);
               pCommAPI->iStageFSid = iStage;
               pCommAPI->iPoolIdRC = ntohl(pQAttr->iPoolId);

               if (iDebug)
               {
                  printf("    file %s staged in FS %d of DM %s (poolId %d)",
                     pcFile, iStage, cNodeMover, pCommAPI->iPoolIdRC);
                  if (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_INCOMPLETE)
                     printf(", still incomplete)\n");
                  else if (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_LOCKED)
                     printf(", already locked)\n");
                  else printf("\n");
               }
            }
            else if ( (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_CACHE) ||
                      (ntohl(pQAttr->iMediaClass) == GSI_CACHE_INCOMPLETE) ||
                      (ntohl(pQAttr->iMediaClass) == GSI_CACHE_LOCKED) )
            {
               iCache = 1;

               /* specify location of cache data */
               strcpy(sFileSystemServ.cOS, "Windows");  /* DDD */
               strcpy(sFileSystemServ.cNode, pQAttr->cNode);
               sFileSystemServ.iFileSystem = pQAttr->iFS;
                                                       /* net format */
               sFileSystemServ.iFileSystem = pQAttr->iFS;
                                                       /* net format */
               strcpy(sFileSystemServ.cArchiveDate, pQAttr->cDateCreate);
                                                     /* archive date */
               strcpy(sFileSystemServ.cArchiveUser, pQAttr->cOwner);
                                                /* user who archived */

               strcpy(cNodeMover, pQAttr->cNode);
               strcpy(pCommAPI->cNodeWC, cNodeMover);
               pCommAPI->iFSidWC = iStage;
               pCommAPI->iPoolIdWC = ntohl(pQAttr->iPoolId);

               if (iDebug)
               {
                  printf("    file %s cached in FS %d of DM %s (poolId %d)",
                     pcFile, iStage, cNodeMover, pCommAPI->iPoolIdWC);
                  if (ntohl(pQAttr->iMediaClass) == GSI_CACHE_INCOMPLETE)
                     printf(", still incomplete)\n");
                  else if (ntohl(pQAttr->iMediaClass) == GSI_CACHE_LOCKED)
                     printf(", already locked)\n");
                  else printf("\n");
               }
            }
            else
            {
               iStage = 0;
               if (iDebug)
                  printf("    file %s not yet staged\n", pcFile);
            }
         } /* iRC(rawQueryFile) > 0 */
      } /* (iObjInit) */
      else
      {
         /******** send request for data mover attr to master ********/

         /* prepare writing of records to server/data mover */
         pCommServ->iAction = htonl(SPM_REQUEST_MOVER);

         if (iDebug) printf(
           "    send command buffer to %s: id: %d, action %d (ATL server %d), len %d\n",
           cServer, ntohl(pCommServ->iIdent),
           ntohl(pCommServ->iAction),
           ntohl(pCommServ->iATLServer),
           ntohl(pCommServ->iCommLen));

         if (iAction == 2)
         {
            pCommAPI->iSynchId = 1;               /* keep DM session */
            pCommServ->iSynchId = htonl(pCommAPI->iSynchId);

            if (iDebug) printf(
               "    keep entry server session for next query\n");
         }

         pcc = (char *) pCommServ;
         iRC = send(iSockMaster, pcc, (unsigned) irawComm, 0);
         if (iRC < 0)
         {
            fprintf(fLogClient,
               "-E- %s: sending command buffer (action %d) to %s\n",
               cModule, ntohl(pCommServ->iAction), cServer); 
            perror("    "); 

            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }

         if (irawComm != iRC)  
         { 
            fprintf(fLogClient,
               "-E- %s: incomplete command buffer sent to %s (%d of %d bytes)\n",
               cModule, cServer, iRC, irawComm);

            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }

         if (iDebug) printf(
            "    command buffer (action %d) sent to %s (%d byte)\n",
            ntohl(pCommServ->iAction), cServer, irawComm);

      } /* (iObjInit == 0) */

      /********* get name of data mover selected by master ***********/

      if (iOnlyQuery)
         ;
      else
         if ( ( (iAction == 1) && ((iStage == 0) || (iCache == 1)) ) ||
              (iAction == 2) )
      {
         if (iDebug) printf(
            "    receiving attributes of data mover selected by master\n");

         iDataMoverSelect = 0;

         while (iDataMoverSelect == 0)
         {
            pcc = (char *) &sDataMoverSelect;
            strcpy(cMsg, "");

            iRC = rawRecvHeadC(iSockMaster, pcc, 0, -1, cMsg);
                                                         /* no check */
            iIdent = ntohl(sDataMoverSelect.iIdent);
            iStatus = ntohl(sDataMoverSelect.iStatus);
            iStatusLen = ntohl(sDataMoverSelect.iStatusLen);

            if (iDebug) printf(
               "    ident %d, status %d, len %d\n",
               iIdent, iStatus, iStatusLen);

            if (iStatus != STA_SWITCH_SERVER)
            {
               if (iRC < 0) fprintf(fLogClient,
                   "-E- %s: receiving data mover attr header\n%s",
                   cModule, cMsg);

               if ( (iStatus == STA_CACHE_FULL) && (iAction == 2) )
               {
                  iStatusLoop++;
                  if ( (iStatusLoop < iStatusLoopMax) &&
                       (iMassSto != 2) )
                     fprintf(fLogClient, 
                        "-W- %s: write cache currently full (select) ... sleep %d s (%d of %d)\n",
                        cModule, iSleepCacheFull, iStatusLoop,
                        iStatusLoopMax);
                  else
                  {
                     fprintf(fLogClient,
                        "-W- %s: write cache currently full, archive to tape\n",
                        cModule);

                     if (iDebug) printf(
                        "    received DM name: %s\n", cMsg);
                     iDataMoverSelect = 1;
                     strcpy(cNodeMover, cMsg);
                     strcpy(pDataMoverSelect->cNode, cMsg);
                     pDataMoverSelect->iATLServer = htonl(0);
                     pDataMoverSelect->iExecStatus = htonl(0);
                     pDataMoverSelect->iWaitTime = htonl(0);

                     pCommAPI->iArchDev = ARCH_TAPE;
                     pCommServ->iArchDev = htonl(pCommAPI->iArchDev);

                     goto gReceivedDM;
                  }

                  /******* send request for DM attr to master ********/

                  /* prepare writing of records to server */
                  pCommServ->iAction = htonl(SPM_REQUEST_MOVER);

                  if (iDebug) printf(
                     "    send command buffer to %s: id: %d, action %d, len %d\n",
                     cServer, pCommAPI->iIdent,
                     ntohl(pCommServ->iAction),
                     ntohl(pCommServ->iCommLen));

                  pcc = (char *) pCommServ;
                  iRC = send(iSockMaster, pcc, (unsigned) irawComm, 0);
                  if (iRC != irawComm)  
                  {
                     if (iRC < 0)
                     { 
                        fprintf(fLogClient,
                           "-E- %s: sending command buffer (action %d) to %s\n",
                           cModule, ntohl(pCommServ->iAction), cServer);
                        perror("    "); 
                     }
                     else fprintf(fLogClient, 
                        "-E- %s: incomplete command buffer sent to %s (%d of %d bytes)\n",
                        cModule, cServer, iRC, irawComm);

                     iSendEOS = 1;
                     iError = 1;
                     goto gClose;
                  }

                  if (iDebug) printf(
                     "    command buffer (action %d) sent to %s (%d byte)\n",
                     ntohl(pCommServ->iAction), cServer, irawComm);

                  continue;
   
               } /* (iStatus == STA_CACHE_FULL && iAction == 2) */

               fprintf(fLogClient, "-E- %s: received error status from %s",
                  cModule, cServer);
               if (iRC)
                  printf("\n%s" , cMsg);
               else
                  printf("\n");

               iSendEOS = 1;
               iError = 1;
               goto gClose;

            } /* (iStatus != STA_SWITCH_SERVER) */

            if (iDebug) printf(
               "    header of data mover attr received (%d bytes)\n",
               iRC);
   
            /* get data part of data mover info */
            pcc = (char *) &(sDataMoverSelect.sDataMoverAttr);
            iBufl = sizeof(srawDataMover) - HEAD_LEN;
            iBuf = iBufl;
            while(iBuf > 0)
            {
               if ( (iRC = recv( iSockMaster, pcc, (unsigned) iBuf, 0 )) < 0 )
               {
                  fprintf(fLogClient, 
                     "-E- %s: receiving data part of data mover info\n",
                     cModule);
                  perror("-E- receiving data part of data mover info");

                  iSendEOS = 1;                    /* may also fail! */
                  iError = 1;
                  goto gClose;
               }

               if (iRC == 0)
               {
                  ii = iBufl - iBuf;
                  fprintf(fLogClient,
                     "-E- %s: connection to sender broken, %d byte of data part of data mover info (%d byte) received\n",
                     cModule, ii, iBufl);

                  iError = 1;
                  goto gClose;
               }

               iBuf -= iRC;
               pcc += iRC;
            }

            pDataMoverSelect = &(sDataMoverSelect.sDataMoverAttr);
            iDataMoverSelect = iBufl / sizeof(srawDataMoverAttr);
                             /* no. of data movers sent: should be 1 */
            strcpy(cNodeMover, pDataMoverSelect->cNode);

            if (iATLServer != (int) ntohl(pDataMoverSelect->iATLServer))
            {
               if ( (iATLServer <= 0) || (iATLServer > MAX_ATLSERVER) )
                  fprintf(fLogClient,
                     "-E- %s: invalid ATL server no. %d received\n",
                     cModule, iATLServer);
               else fprintf(fLogClient, 
                  "-W- %s: unexpected ATL server of selected DM: %d, expected %d\n",
                  cModule,
                  ntohl(pDataMoverSelect->iATLServer), iATLServer);

               iATLServer = ntohl(pDataMoverSelect->iATLServer);
               pCommAPI->iATLServer = iATLServer;
               pCommServ->iATLServer = htonl(iATLServer);
            }

            if (iDebug)
            {
               printf("    data part of data mover info received (%d byte)\n",
                  iBufl);
               printf("    %d data mover selected: %s for ATL server %d\n",
                  iDataMoverSelect, cNodeMover, iATLServer);
            }
            else if (iAction == 1) printf(
              "-I- staging file to data mover %s (ATL server %d) ...\n",
              cNodeMover, iATLServer);

         } /* while (iDataMoverSelect == 0) */

         if (iDataMoverSelect != 1)
         {
            fprintf(fLogClient, 
               "-W- expected 1 selected data mover, ignore remaining %d\n",
               --iDataMoverSelect);
             iDataMoverSelect = 1;
         }
      } /* ( ( (iAction == 1) && ((iStage == 0) || (iCache == 1)) ) ||
             (iAction == 2) */

gReceivedDM:
      /* update communication buffer with query infos */
      pCommServ->iATLServer = htonl(iATLServer);

      if (iAction == 1)
      {
         pCommServ->iFileType = pQAttr->iFileType;  /* is net format */
         pCommServ->iBufsizeFile = pQAttr->iBufsizeFile;
         pCommServ->iFileSize = pQAttr->iFileSize;
         pCommServ->iFileSize2 = pQAttr->iFileSize2;
         pCommServ->iObjHigh = pQAttr->iObjHigh;
         pCommServ->iObjLow = pQAttr->iObjLow;
         
         pCommServ->iStageFSid = htonl(0);
         pCommServ->iFSidWC = htonl(0);
         if (iDebug) printf("    iMediaClass %d, disk FS %d\n",
            ntohl(pQAttr->iMediaClass), iStage);
         if (iStage)
         {
            if ( (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_STAGE) ||
                 (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_LOCKED) ||
                 (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_INCOMPLETE) )
               pCommServ->iStageFSid = pQAttr->iFS;
            else if ( (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_CACHE) ||
                      (ntohl(pQAttr->iMediaClass) == GSI_CACHE_LOCKED) ||
                      (ntohl(pQAttr->iMediaClass) == GSI_CACHE_INCOMPLETE) )
               pCommServ->iFSidWC = pQAttr->iFS;
         }
      }
      else
      {
         pCommServ->iFileType = htonl(STREAM);
         pCommServ->iBufsizeFile = htonl(0);
         pCommServ->iFileSize = htonl(0);
         pCommServ->iFileSize2 = htonl(0);
         pCommServ->iObjHigh = htonl(0);
         pCommServ->iObjLow = htonl(0);
         pCommServ->iStageFSid = htonl(0);
         pCommServ->iFSidWC = htonl(0);
      }

      /* provide query infos to other API procedures */
      pCommAPI->iATLServer = iATLServer;
      pCommAPI->iFileType = ntohl(pCommServ->iFileType);
      pCommAPI->iBufsizeFile = ntohl(pCommServ->iBufsizeFile);
      pCommAPI->iFileSize = ntohl(pCommServ->iFileSize);
      pCommAPI->iFileSize2 = ntohl(pCommServ->iFileSize2);
      pCommAPI->iObjHigh = ntohl(pCommServ->iObjHigh);
      pCommAPI->iObjLow = ntohl(pCommServ->iObjLow);
      pCommAPI->iStageFSid = ntohl(pCommServ->iStageFSid);
      pCommAPI->iFSidWC = ntohl(pCommServ->iFSidWC);

      if (iStage)
      {
         pCommServ->iExecStatus = htonl(0);
         pCommAPI->iExecStatus = 0;
      }
      else
      {
         pCommServ->iExecStatus = pDataMoverSelect->iExecStatus;
         pCommAPI->iExecStatus = ntohl(pDataMoverSelect->iExecStatus);

         pCommServ->iWaitTime = pDataMoverSelect->iWaitTime;
         pCommAPI->iWaitTime = ntohl(pDataMoverSelect->iWaitTime);
      }

      /********************* connect to data mover *******************/

      if (iOnlyQuery == 0)
      {
         if (iDebug) printf(
            "    connecting to data mover %s:%d\n",
            cNodeMover, iPortMover);

         iRC = rconnect(cNodeMover, iPortMover,
                        &iMaxConnect, &iSockMover);
         if (iRC)
         {
            fprintf(fLogClient,
               "-E- %s: cannot connect to data mover %s:%d (ATL server %d)\n",
               cModule, cNodeMover, iPortMover, iATLServer);
            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }

         fprintf(fLogClient,
            "-I- successfully connected to data mover %s:%d (ATL server %d)",
            cNodeMover, iPortMover, iATLServer);
         if (iDebug)
         {
            if (iMaxConnect)
               fprintf(fLogClient, " after %ds", iMaxConnect);
            fprintf(fLogClient, " (socket %d)\n", iSockMover);
         }
         else
            fprintf(fLogClient, "\n");

         strcpy(pcurAPIFile->cNodeMover, cNodeMover);
         pcurAPIFile->iSockMover = iSockMover;
         strcpy(cServer, "data mover");
      }

      /******************** close connection to master ***************/

      if (iOnlyQuery)
      {
         iSendEOS = 1;
         goto gClose;
      }

      /* as read from data mover in analysis may take a long time!
         but keep in case of DAQPool/ArchivePool for further queries */
      if ( (iSockMaster >= 0) && (iAction == 1) )
      {
         if (iDebug)
            printf("-D- close connection %d to master\n", iSockMaster);
         iRC = shutdown(iSockMaster, 2);
         if (iDebug)
            printf("    rc shutdown = %d\n", iRC);
         iRC = close(iSockMaster);
         if (iDebug)
            printf("    rc close = %d\n", iRC);

         iSockMaster = -1;
         pcurAPIFile->iSockMaster = -1;

      } /* (iSockMaster >= 0 && iAction == 1) */

   } /* (iMassSto) */

   if (iAction == 1)
   {
      pCommAPI->iAction = RETRIEVE_RECORD;
      strcpy(pCommAPI->cNodeRC, cNodeMover);

      if (iCache == 1)
      {
         /* request file copy from write cache to stage pool */
         pCommAPI->iCommLen += irawFileSystem;
         pCommAPI->iFSidWC = iStage;
         /* pCommAPI->cNodeWC already set */
      }
   } /* read */
   else
   {
      /* prepare writing of records to server/data mover */
      pCommAPI->iAction = ARCHIVE_RECORD;
   }

   pCommServ->iAction = htonl(pCommAPI->iAction);
   pCommServ->iCommLen = htonl(pCommAPI->iCommLen);

   pCommServ->iPoolIdWC = htonl(pCommAPI->iPoolIdWC);
   strcpy(pCommServ->cNodeWC, pCommAPI->cNodeWC);
   pCommServ->iFSidWC = htonl(pCommAPI->iFSidWC);

   pCommServ->iPoolIdRC = htonl(pCommAPI->iPoolIdRC);
   strcpy(pCommServ->cNodeRC, pCommAPI->cNodeRC);
   pCommServ->iStageFSid = htonl(pCommAPI->iStageFSid);

   if (iObjInit)
   {
gNextCmdOpen:
      if (iDebug)
      {
         printf(
           "    send command buffer to %s: id: %d, action %d, data len %d\n",
            cServer, pCommAPI->iIdent,
            ntohl(pCommServ->iAction),
            ntohl(pCommServ->iCommLen));
         if (iMassSto)
         {
            pCommServ->iFileSize = ntohl(pCommServ->iFileSize);
            pCommServ->iFileSize2 = ntohl(pCommServ->iFileSize2);
            plFileSizeC = (unsigned long *) &(pCommServ->iFileSize);

            printf("    filesize %lu bytes, record size %d",
               *plFileSizeC, ntohl(pCommServ->iBufsizeFile));
            if (iAction == 1) printf(
               ", poolIdRC %d, poolIdWC %d, obj id %d-%d\n",
               ntohl(pCommServ->iPoolIdRC), ntohl(pCommServ->iPoolIdWC),
               ntohl(pCommServ->iObjHigh), ntohl(pCommServ->iObjLow));
            else
               printf(", poolId %d\n", ntohl(pCommServ->iPoolIdWC));

            /* reconvert to net format */
            pCommServ->iFileSize = htonl(pCommServ->iFileSize);
            pCommServ->iFileSize2 = htonl(pCommServ->iFileSize2);
         }
      }

      pcc = (char *) pCommServ;
      if ( (iRC = send(iSockMover, pcc, (unsigned) irawComm, 0 )) < 0 ) 
      {
         fprintf(fLogClient, "-E- %s: sending command buffer to %s (rc = %d)\n",
            cModule, cServer, iRC); 
         perror("    "); 

         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }

      if (irawComm != iRC)  
      { 
         fprintf(fLogClient,
            "-E- %s: incomplete command buffer sent to %s (%d of %d bytes)\n",
            cModule, cServer, iRC, irawComm);

         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }

      if (iDebug) printf(
        "    command buffer sent to %s (%d byte)\n", cServer, irawComm);

      /* append filesystem buffer */
      if (iCache == 1)
      {
         pcc = (char *) &sFileSystemServ;
         iRC = send(iSockMover, pcc, (unsigned) irawFileSystem, 0);
         if (irawFileSystem != iRC)
         {
            if (iRC < 0)
            {
               fprintf(fLogClient,
                  "-E- %s: sending filesystem buffer to data mover %s failed\n",
                  cModule, cNodeMover);
               perror("    ");
            }
            else fprintf(fLogClient, 
               "-E- %s: incomplete filesystem buffer sent to data mover %s\n",
               cModule, cNodeMover);

            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }

        if (iDebug) printf(
           "    write cache info (%d bytes) sent to data mover %s\n",
           irawFileSystem, cNodeMover);

      } /* (iCache == 1) */

      /************* look for reply from server/data mover ***********/

      pcc = (char *) &sStatus;
      iStatusOkay = 0;
      iStatusLoop = 0;
      while (iStatusOkay == 0)
      {
         iStatusLoop++;

         memset(&sStatus, 0X00, sizeof(srawStatus));
         iRC = rawRecvStatus(iSockMover, pcc);
         iStatus = sStatus.iStatus;
         if (iRC != HEAD_LEN)
         {
            if (iRC < HEAD_LEN)
            {
               fprintf(fLogClient, "-E- %s: receiving status buffer from %s",
                  cModule, cServer);
               if (iRC > 0)
                  fprintf(fLogClient, ": (%d byte)\n", iRC);
               else
                  fprintf(fLogClient, "\n");
            }
            else                           /* received error message */
            {
               if ( (iStatus == STA_CACHE_FULL) && (iAction == 2) )
               {
                  if ( (iStatusLoop < iStatusLoopMax) &&
                       (iMassSto != 2) )     /* no delay for DAQPool */
                  {
                     fprintf(fLogClient,
                        "-W- %s: write cache currently full (create) ... sleep %d s (%d of %d)\n",
                        cModule, iSleepCacheFull,
                        iStatusLoop, iStatusLoopMax);
                     continue;
                  }
                  else
                  {
                     fprintf(fLogClient,
                        "-W- %s: write cache currently full: archive to tape\n",
                        cModule);
                     pCommAPI->iAction = ARCHIVE_RECORD;
                     pCommAPI->iArchDev = ARCH_TAPE;
                     pCommServ->iAction = htonl(pCommAPI->iAction);
                     pCommServ->iArchDev = htonl(pCommAPI->iArchDev);

                     goto gNextCmdOpen;
                  }
               } /* (iStatus == STA_CACHE_FULL && iAction == 2) */
               else if ( (iStatus == STA_SWITCH_SERVER) &&
                       (iAction == 2) )
               {
                  strcpy(cNodeMover, sStatus.cStatus);
   
                  fprintf(fLogClient, 
                     "-W- write cache on DM %s full, switch to %s\n",
                     pcurAPIFile->cNodeMover, cNodeMover);

                  if (iDebug) printf(
                     "    close connection %d to data mover %s\n",
                     iSockMover, pcurAPIFile->cNodeMover);
                  iRC = shutdown(iSockMover, 2);
                  if (iDebug)
                     printf("    rc shutdown = %d\n", iRC);
                  iRC = close(iSockMover); 
                  if (iDebug)
                     printf("    rc close = %d\n", iRC);

                  /************* connect to new data mover ***********/

                  if (iDebug) printf(
                     "    connecting to data mover %s:%d\n",
                     cNodeMover, iPortMover);

                  iRC = rconnect(cNodeMover, iPortMover,
                                 &iMaxConnect, &iSockMover);
                  if (iRC)
                  {
                     fprintf(fLogClient, 
                        "-E- %s: cannot connect to new data mover %s:%d (ATL server %d)\n",
                        cModule, cNodeMover, iPortMover, iATLServer);
                     iError = -2;
                  }

                  fprintf(fLogClient, 
                     "-I- successfully connected to new data mover %s:%d (ATL server %d)",
                     cNodeMover, iPortMover, iATLServer);
                  if (iMaxConnect)
                     fprintf(fLogClient, " after %ds", iMaxConnect);
                  if (iDebug)
                     printf("    (socket %d)\n", iSockMover);
                  else
                     fprintf(fLogClient, "\n");

                  strcpy(pcurAPIFile->cNodeMover, cNodeMover);
                  pcurAPIFile->iSockMover = iSockMover;

                  goto gNextCmdOpen;

               } /* (iStatus == STA_SWITCH_SERVER && iAction == 2) */
               else
               {
                  fprintf(fLogClient, "-E- %s: message received from %s:\n",
                     cModule, cServer);
                  fprintf(fLogClient, "    %s\n", sStatus.cStatus);
                  iError = 2;
               }
            } /* (iRC >= HEAD_LEN) */

            iSendEOS = 1;
            goto gClose;
         }

         iStatusOkay = 1;
   
      } /* while (iStatusOkay == 0) */

      if (iDebug)
      {
         printf("    status (%d) received from %s (%d bytes", 
                iStatus, cServer, iRC);
         if (iStatusLoop > 1)
            printf(", iter %d)\n", iStatusLoop);
         else
            printf(")\n");
      }
   } /* (iObjInit) */

   iError = 0;

gClose:
   if ( (iSockMaster >= 0) &&
        ((iAction == 1) || (iError)) )
   {
      /* stop master server process in case of read cache or error */
      if (iSendEOS)
      {
         iStatus = STA_END_OF_SESSION;
         iRC = rawSendStatus(iSockMaster, iStatus, NULL);
         if (iRC <= 0) fprintf(fLogClient,
            "-E- %s: sending status buffer to entry server (EOS)\n", cModule);
         else if (iDebug) printf(
            "    status buffer (EOS) sent to entry server (%d bytes)\n", iRC);
      }

      if (iDebug)
         printf("    close connection %d to entry server\n", iSockMaster);
      iRC = shutdown(iSockMaster, 2);
      if (iDebug)
         printf("    irc shutdown = %d\n", iRC);
      iRC = close(iSockMaster);
      if (iDebug)
         printf("    irc close = %d\n", iRC);

   } /* (iSockMaster >= 0 && iAction == 1) */

   if (iError)
   {
gError:
      if (iSockMover >= 0)
      {
         if (iSendEOS)
         {
            /* stop server/data mover process */
            iStatus = STA_END_OF_SESSION;
            iRC = rawSendStatus(iSockMover, iStatus, NULL);
            if (iRC <= 0) fprintf(fLogClient,
               "-E- %s: sending status buffer (EOS) to %s\n", cModule, cServer);
            else if (iDebug) printf(
               "    status buffer (EOS) sent to %s (%d bytes)\n",
               cServer, iRC);
         }

         if (iDebug) printf(
            "    close connection %d to %s\n", iSockMover, cServer);
         iRC = shutdown(iSockMover, 2);
         if (iDebug)
            printf("    rc shutdown = %d\n", iRC);
         iRC = close(iSockMover); 
         if (iDebug)
            printf("    rc close = %d\n", iRC);
      }

      if (iFileId >= 0)
      {
         free(pAPIFile[iFileId]);
         pAPIFile[iFileId] = NULL;
         iFileCount--;
      }

   } /* (iError) */

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   if (iError)
      return -1;
   else
      return iFileId;

} /* rfio_open_gsidaq */

/********************************************************************
 * rfio_endfile:
 *   close remote file in GSI mass storage,
 *   but keep connection to mass storage data mover
 *
 * created 16. 7.2004, Horst Goeringer
 ********************************************************************
 */

int rfio_endfile(int iFileId)                             /* file id */
{
   char cModule[32] = "rfio_endfile";
   int iDebug = 0;
   int iRC = 0;
   int iSockMover;                 /* socket for connection to mover */
   int iStatus = STA_END_OF_SESSION;
   int ii = 0;

   char cServer[16] = "data mover";
   char *pcc;

   srawStatus sStatus;
   srawAPIFile *pcurAPIFile;
   srawComm *pCommAPI;  /* communication buffer in API control block */
   srawComm *pCommServ;            /* copy for servers in net format */
   srawCopyCache *pCopyCacheServ;
                          /* infos in net format for copy of WC data */

   if (iDebug) printf(
      "\n-D- begin %s: close remote file %d\n", cModule, iFileId);
   fflush(stdout);

   if (iFileCount == 0)
   {
      fprintf(fLogClient,
         "-E- %s: no remote file available for close\n", cModule);
      return -1;
   }

   while (ii < iFileMax)
   {
      if (pAPIFile[ii])
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }
      ii++;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient, "-E- %s: remote file %d cannot be closed: not found\n",
         cModule, iFileId);
      return -1;
   }

   pcurAPIFile = pAPIFile[iFileId];
   pCommAPI = &(pcurAPIFile->sCommAPI);
   pCommServ = &(pcurAPIFile->sCommServ);
   pCopyCacheServ = &(pcurAPIFile->sCopyCacheServ);

   pcurAPIFile->iBufnoServ = 0;   /* RESET */
   iSockMover = pcurAPIFile->iSockMover;
   if (iSockMover >= 0) 
   {
      /* stop data mover process */
      iStatus = STA_END_OF_FILE;
      iRC = rawSendStatus(iSockMover, iStatus, NULL);
      if (iRC <= 0)
         fprintf(fLogClient, "-E- %s: sending status buffer (EOF) to %s\n",
            cModule, cServer);
      else if (iDebug) printf(
         "    status buffer (EOF) sent to %s (%d bytes)\n",
         cServer, iRC);

      memset(&sStatus, 0X00, sizeof(srawStatus));
      pcc = (char *) &sStatus;
      iRC = rawRecvStatus(iSockMover, pcc);
      if (iRC < HEAD_LEN)
      {
         fprintf(fLogClient, "-E- %s: receiving status buffer\n", cModule);
         iRC = -1;
         goto gErrorEndFile;
      }

      iStatus = sStatus.iStatus;
      if (iDebug) printf(
         "    status buffer (%d) received (%d byte)\n",
         iStatus, iRC);

      if (iRC > HEAD_LEN)
      {
         if (iStatus == STA_CACHE_COPY)
            fprintf(fLogClient, sStatus.cStatus);
         else
         {
            fprintf(fLogClient,
               "-W- %s: message received from server:\n", cModule);
            fprintf(fLogClient, sStatus.cStatus);

            if (iStatus != STA_CACHE_COPY_ERROR)
            {
               iRC = -1;
               goto gErrorEndFile;
            }
         }
      } /* (iRC > HEAD_LEN) */

      if (pcurAPIFile->iMassSto)
      {
         if ( (pcurAPIFile->iOpMode != 2) &&      /* no display mode */
              (iDebug) )
            fprintf(fLogClient,
               "-I- gStore file %s%s%s closed\n", 
               pCommAPI->cNamefs, pCommAPI->cNamehl, pCommAPI->cNamell);
      }
      else fprintf(fLogClient,
         "-I- remote output file %s%s%s closed\n", 
         pCommAPI->cNamefs, pCommAPI->cNamehl, pCommAPI->cNamell);

   } /* (iSockMover >= 0) */

   if (iDebug)
   {
      printf(
         "    remote file control block %d still active (overall %d)\n",
         iFileId, iFileCount);
      printf("-D- end %s\n\n", cModule);
   }

   iRC = 0;

gErrorEndFile:
   return iRC;

} /* rfio_endfile */

/*********************************************************************
 * rfio_read: read remote file in GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 *********************************************************************
 */

ssize_t rfio_read(int iFileId,                            /* file id */
                  char *pcBuffer,                     /* data buffer */
                  size_t iItems)             /* number of data bytes */
{
   char cModule[32] = "rfio_read";
   int iDebug = 0;
   int iRC;

   int iSockMover;                 /* socket for connection to mover */
   int iSeekMode;
   int iOffset;
   int iBufsizeRead;
   int iBufnoServ;      /* no. of buffers transfered from/to server */
   int iDataServ;         /* no. of bytes transfered from/to server */

   int iStatus;
   int iStatusLen;
   int iBufsizeRecv;

   char *pcc;
   int ii = 0;
   int iBuf, iBufl;
   int *pint;

   srawAPIFile *pcurAPIFile;
   srawComm *pCommAPI;  /* communication buffer in API control block */
   srawComm *pCommServ;            /* copy for servers in net format */

   if (iDebug)
      printf("\n-D- begin %s: fileId %d\n", cModule, iFileId);

   if (iFileCount == 0)
   {
      fprintf(fLogClient, "-E- %s: no remote file open for read\n", cModule);
      return -1;
   }

   while (ii < iFileMax)
   {
      if (pAPIFile[ii])
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }
      ii++;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient, "-E- %s: remote file %d cannot be read: not opened\n",
         cModule, iFileId);
      return -1;
   }

   if (iDebug)
      printf("    open buffer %d, fileId %d\n", ii, iFileId);

   pcurAPIFile = pAPIFile[iFileId];
   pCommAPI = &(pcurAPIFile->sCommAPI);
   pCommServ = &(pcurAPIFile->sCommServ);

   iBufnoServ = pcurAPIFile->iBufnoServ;
   iDataServ = pcurAPIFile->iDataServ;

   iBufnoServ++;
   if (iDebug)
   {
      printf("    remote file control block %d selected\n", ii);
      printf("    file %d, buffer %d: read %d bytes\n",
             iFileId, iBufnoServ, iItems);
   }

   iBufsizeRead = pcurAPIFile->iBufsizeAct;
   if (iDebug)
   {
      if (iBufsizeRead != iItems)
         printf( "    new buffer size %d\n", iItems);
      printf("    send request for new buffer\n");
   }

   iSockMover = pcurAPIFile->iSockMover;
   iSeekMode = pcurAPIFile->iSeekMode;
   iOffset = pcurAPIFile->iOffset;
   iRC = rawSendRequest(iSockMover, iSeekMode, iOffset, (signed) iItems);
   if (iRC != 0)
   {
      fprintf(fLogClient, "-E- %s: sending request for next data buffer\n",
         cModule);
      return -1;
   }

   /* reset seek options to default (no seek) */
   if (iSeekMode != -1)
   {
      pcurAPIFile->iSeekMode = -1;
      pcurAPIFile->iOffset = 0;
   }

   pcurAPIFile->iBufsizeAct = (int) iItems;    /* keep current value */

   /********************** retrieve next buffer **********************/

   /* get size retrieve buffer */
   pcc = (char *) &iBufsizeRecv;
   iBuf = iint;
   while(iBuf > 0)
   {
gRetryLen:
      if ( (iRC = recv( iSockMover, pcc, (unsigned) iBuf, 0 )) < 0 )
      {
         fprintf(fLogClient, 
            "-E- %s: receiving data length from server (buffer %d)",
            cModule, iBufnoServ);
         perror("");
         goto gRetryLen;
      }

      if (iRC == 0)
      {
         ii = iint - iBuf;
         fprintf(fLogClient,
            "-E- %s: connection to sender broken, %d byte of data length (%d byte) received (buffer %d)\n",
            cModule, ii, iint, iBufnoServ);

         return -1;
      }

      iBuf -= iRC;
      pcc += iRC;

      if (iDebug == 2)
      {
         printf("%d.", iRC);
         fflush(stdout);
      }
   } /* recv loop data length */

   iBufsizeRecv = ntohl(iBufsizeRecv);
   if (iDebug == 2)
       printf("%d_", iBufsizeRecv);

   if (iBufsizeRecv < 0)
   {
      if (iBufsizeRecv != IDENT_STATUS)
      {
         fprintf(fLogClient, "-E- %s: invalid status header received (%d)\n",
            cModule, iBufsizeRecv);
         return -1;
      }
      if (iDebug)
         printf("-D- status header found\n");

      /* receive remainder of status header */
      pcc = pcBuffer;
      iBuf = HEAD_LEN - 4;                   /* iIdent already read */
      iBufl = iBuf;
      while(iBuf > 0)             /* get remainder of status header */
      {
         if ( (iRC = recv( iSockMover, pcc, (unsigned) iBuf, 0 )) < 0 )
         {
           fprintf(fLogClient,
              "-E- %s: receiving remainder of status header from server (buffer %d)",
              cModule, iBufnoServ);
           perror("    ");

           return -1;
         }

         if (iRC == 0)
         {
            ii = iBufl - iBuf;
            fprintf(fLogClient,
               "-W- %s: connection to sender broken, %d byte of remainder of status header (%d byte) received (buffer %d)\n",
               cModule, ii, iBufl, iBufnoServ);

            return -1;
         }

         if (iDebug == 2)
            printf("%d.", iRC);

         iBuf -= iRC;
         pcc += iRC;
      }

      pint = (int *) pcBuffer;              /* points to status word */
      iStatus = ntohl(*pint);                     /* status received */
      if ( (iStatus != STA_END_OF_FILE)    &&
           (iStatus != STA_END_OF_SESSION) &&
           (iStatus != STA_ERROR) &&
           (iStatus != STA_ERROR_EOF) )
      {
         fprintf(fLogClient, "-E- %s: unexpected status from server: %d\n",
            cModule, iStatus);
         return -1;
      }

      if ( (iStatus == STA_END_OF_FILE) ||
           (iStatus == STA_END_OF_SESSION) )
      {
         if (iDebug) printf("    EOF on input\n");

         if (iDataServ == 0)                          /* empty file */
         {
            fprintf(fLogClient, "-E- %s: %s%s%s empty\n",
               cModule, pCommServ->cNamefs,
               pCommServ->cNamehl, pCommServ->cNamell);
            return -1;
         }
         else return 0;
      }
      else
      {
         pint++;                           /* points to status size */
         iStatusLen = ntohl(*pint);
         if (iStatusLen > 0)
         {
            iRC = rawRecvError(iSockMover, iStatusLen, pcc);
            if (iRC < 0)
            {
              fprintf(fLogClient,
                 "-E- %s: receiving status message from server, rc = %d\n",
                 cModule, iRC);
              return -1;
            }

            if ( (iStatus == STA_ERROR) ||
                 (iStatus == STA_ERROR_EOF) )
            {
               fprintf(fLogClient, 
                  "-E- %s: received error status from server:\n",
                  cModule);
               fprintf(fLogClient, "%s\n", pcc);
               return -1;
            }

            /* warning from server */
            printf("%s\n", pcc);

         } /* (iStatusLen > 0) */
      } /* (iStatus != STA_END_OF_FILE) && 
           (iStatus != STA_END_OF_SESSION) */
   } /* (iBufsizeRecv < 0) */
   else if (iBufsizeRecv != iItems)
   {
      if (iDebug)
      {
         if (iBufsizeRecv) printf(
            "-W- requested %d bytes, received buffer length %d bytes\n",
            iItems, iBufsizeRecv);
         else printf("-W- EOF reached\n");
      }
      iItems = (size_t) iBufsizeRecv;
   }

   /* recv loop over buffers of size iBufsizeRecv */
   iBuf = iBufsizeRecv;
   pcc = pcBuffer;
   while(iBuf > 0)
   {
gRetryData:
      if ( (iRC = recv( iSockMover, pcc, (unsigned) iBuf, 0 )) < 0 )
      {
         if (iDebug) fprintf(fLogClient, 
            "-E- %s: receiving data from server (buffer %d)\n",
            cModule, iBufnoServ);
         perror("EEE ");
         goto gRetryData;
      }

      if (iRC == 0)
      {
         ii = iBufsizeRecv - iBuf;
         fprintf(fLogClient,
            "-W- %s: connection to sender broken, %d byte of data buffer (%d byte) received (buffer %d)\n",
            cModule, ii, iBufsizeRecv, iBufnoServ);

         return -1;
      }

      if (iDebug == 2)
      {
         printf("%d:", iRC);
         fflush(stdout);
      }

      iBuf -= iRC;
      pcc += iRC;
      pcurAPIFile->iDataServ += iRC;

   } /* while(iBuf > 0) */

   if (iBufsizeRecv)
   {
      pcurAPIFile->iBufnoServ++;
      if (iDebug == 2)
         printf(" buffer %d received\n", pcurAPIFile->iBufnoServ);
   }

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return iBufsizeRecv;

} /* rfio_read */

/*********************************************************************
 * rfio_write: write remote file in GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 *********************************************************************
 */

ssize_t rfio_write(int iFileId,                           /* file id */
                   const char *pcBuffer,              /* data buffer */
                   size_t iItems)            /* number of data bytes */
{
   char cModule[32] = "rfio_write";
   int iDebug = 0;

   int iRC;
   char *pcc;
   int ii = 0;
   int iBuf;

   int iSockMover;                 /* socket for connection to mover */
   int iBufsizeWrite;
   int iBufnoServ;       /* no. of buffers transfered from/to server */

   srawAPIFile *pcurAPIFile;

   srawComm *pCommAPI;
   srawComm *pCommServ;            /* copy for servers in net format */

   if (iDebug)
   {
      printf("\n-D- begin %s\n", cModule);
      fflush(stdout);
   }

   if (iItems <= 0)
   {
      fprintf(fLogClient,
         "-W- invalid no. of bytes to write: %d\n", iItems);
      return 0;
   }

   if (iFileCount == 0)
   {
      fprintf(fLogClient, "-E- %s: no remote file open for write\n", cModule);
      return -1;
   }

   ii = 0;
   while (ii < iFileMax)
   {
      if (pAPIFile[ii])
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }
      ii++;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient,
         "-E- %s: remote file %d cannot be written: not opened\n",
         cModule, iFileId);
      return -1;
   }

   pcurAPIFile = pAPIFile[iFileId];
   pCommServ = &(pcurAPIFile->sCommServ);
   pCommAPI = &(pcurAPIFile->sCommAPI);

   iBufnoServ = pcurAPIFile->iBufnoServ;
   iBufnoServ++;

   if (iDebug)
   {
      printf("    remote file control block %d selected\n", ii);
      printf("    file %d, buffer %d: write %d bytes\n",
             iFileId, iBufnoServ, iItems);
   }

   if (iItems > pCommAPI->iBufsizeFile)
   {
      pCommAPI->iBufsizeFile = (int) iItems;
      pCommServ->iBufsizeFile = htonl(pCommAPI->iBufsizeFile);
   }

   if (iDebug)
   {
      if (pcurAPIFile->iBufsizeAct != iItems)
         printf( "    new buffer size %d\n", iItems);
      printf("    send new buffer\n");
   }


   /*********************** write next buffer ************************/

   iSockMover = pcurAPIFile->iSockMover;
   pcurAPIFile->iBufsizeAct = (int) iItems;    /* keep current value */
   iBufsizeWrite = htonl( (int) iItems);
   iBuf = iint;
   pcc = (char *) &iBufsizeWrite;
   while (iBuf)
   {
      iRC = send( iSockMover, pcc, (unsigned) iBuf, 0 );
      if (iRC <= 0) 
      {
         if (iRC == 0) fprintf(fLogClient,
            "-E- %s: sending data length to server broken\n", cModule);
         else
         {
            fprintf(fLogClient,
               "-E- %s: sending data length to server (buffer %d)",
               cModule, iBufnoServ);
            perror("    ");
         }
         return -1;
      }

      iBuf -= iRC;
      pcc += iRC;

      if (iDebug == 2)
      {
         printf("%d(%d)_", iRC, iItems);
         fflush(stdout);
      }

   } /* while (iBuf) */

   iBuf = (int) iItems;
   pcc = (char *) pcBuffer;
   while (iBuf)
   {
      iRC = send( iSockMover, pcc, (unsigned) iBuf, 0 );
      if (iRC <= 0) 
      {
         if (iRC == 0) fprintf(fLogClient,
            "-E- %s: sending data to server broken\n", cModule);
         else
         {
            fprintf(fLogClient, 
               "-E- %s: sending data to server (buffer %d, %d bytes)",
               cModule, iBufnoServ, iBuf);
            perror("    ");
         }
         return -1;
      }

      iBuf -= iRC;
      pcc += iRC;
      pcurAPIFile->iDataServ += iRC;

      if (iDebug == 2)
      {
         printf("%d.", iRC);
         fflush(stdout);
      }

   } /* while (iBuf) */

   if (iItems)
   {
      pcurAPIFile->iBufnoServ++;
      if (iDebug == 2)
         printf(" buffer %d sent\n", pcurAPIFile->iBufnoServ);
   }

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return (ssize_t) iItems;

} /* rfio_write */

/*********************************************************************
 * rfio_newfile: continue with next remote file in GSI mass storage
 *
 * created  2. 8.2004, Horst Goeringer
 *********************************************************************
 */

int rfio_newfile(int iFileId,
                 char *pcFile)                          /* file name */
{
   char cModule[32] = "rfio_newfile";
   int iDebug = 0;
   int iRC;
   int ii = 0;
   int iArchLen = 0;
   int iPathLen = 0;
   int iFileLen = 0;
   int iCopyMode = 0;             /* >0: write data to WC and dataFS */ 
   int iCopyLen = 0;       /* >0: copy parameters already sent to DM */
   int iCopyBuffer = 0;        /* >0: send copy parameters now to DM */

   int iMassSto = -1;/* =1: connect to GSI mass storage (ArchivePool) 
                        =2: connect to GSI mass storage (DAQPool)
                        =0: connect to RFIO read/write server        */
   int iMaxConnect = 0;            /* try connection to servers once */
   int iSockMover;                 /* socket for connection to mover */
   char cNodeMover[MAX_NODE] = "";
   int inewPath = 0;
   int iError = 0;
   int iStatus = 0;
   int iStatusOkay = 0;
   int iStatusLoop = 0;

   char *pcc, *pcc1, *pcc2;
   char cTemp[STATUS_LEN] = "";

   char cNamefs[MAX_OBJ_FS] = "", *pcNamefs;       /* filespace name */
   char cNamehl[MAX_OBJ_HL] = "", *pcNamehl;      /* high level name */
   char cNamell[MAX_OBJ_LL] = "", *pcNamell;      /* low  level name */

   srawStatus sStatus;
   void *pBufQuery;
   srawQueryResult sQuery;
   srawObjAttr *pQAttr;

   srawAPIFile *pcurAPIFile;
   srawComm *pCommAPI;  /* communication buffer in API control block */
   srawComm *pCommServ;            /* copy for servers in net format */
   srawCopyCache *pCopyCacheServ;
                          /* infos in net format for copy of WC data */

   pcNamefs = cNamefs;
   pcNamehl = cNamehl;
   pcNamell = cNamell;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (iFileCount == 0)
   {
     fprintf(fLogClient,
        "-E- %s: no open connection to server existing\n", cModule);
     iError = -1;
     goto gEndNewFile;
   }

   ii = 0;
   while (ii < iFileMax)
   {
      if (pAPIFile[ii])
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }
      ii++;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient, "-E- %s: no open connection to server\n", cModule);
      iError = -1;
      goto gEndNewFile;
   }

   if (iDebug)
      printf("    requested API control block %d\n", iFileId);

   pcurAPIFile = pAPIFile[iFileId];
   pCommAPI = &(pcurAPIFile->sCommAPI);
   pCommServ = &(pcurAPIFile->sCommServ);
   pCopyCacheServ = &(pcurAPIFile->sCopyCacheServ);

   if ( (pcurAPIFile->iAction == 1) ||
        (pCommAPI->iAction == QUERY_RETRIEVE_RECORD) ||
        (pCommAPI->iAction == RETRIEVE_RECORD) )
   {
      fprintf(fLogClient, "-E- %s for read not supported\n", cModule);
      iError = -1;
      goto gEndNewFile;
   }
   else if (pcurAPIFile->iAction == 2)
   {
      if (iDebug)
         printf("    connection is open for write\n");
   }
   else
   {
      fprintf(fLogClient, "-E- %s: connection is open for unexpected action %d\n",
         cModule, pcurAPIFile->iAction);
      iError = -1;
      goto gEndNewFile;
   }

   if (iDebug)
      printf("    check name parts in %s\n", pcFile);

   pcc1 = (char *) strrchr(pcFile, *pcDevDelim);
   if (pcc1 == NULL)
   {
      fprintf(fLogClient,
         "-E- %s: invalid remote file name %s: node name missing\n",
         cModule, pcFile);
      iError = -1;
      goto gEndNewFile;
   }

   strcpy(cTemp, pcFile);      /* local copy of new remote file name */
   pcc = (char *) strchr(pcFile, *pcObjDelim);
   if (pcc == NULL)
   {
      fprintf(fLogClient, "-E- %s: invalid remote file name %s\n",
         cModule, pcFile);
      iError = -1;
      goto gEndNewFile;
   }

   ii = pcc1 - pcc;
   if (ii < 0)
      ii = -ii;
   if (ii != 1)
   {
      fprintf(fLogClient,
            "-E- %s: invalid remote file name %s - possibly leading '/' in archive name missing\n",
         cModule, pcFile);
      iError = -1;
      goto gEndNewFile;
   }

   if (strncmp(cTemp, "rfiocopy:", 9) == 0)
   {
      iMassSto = 0;
      if (iDebug)
         printf("    copy via RFIO server\n");
   }
   else if (strncmp(cTemp, "rfiodaq:", 8) == 0)
   {
      iMassSto = 2;
      if (iDebug)
         printf("    copy to mass storage (DAQPool)\n");
   }
   else if (strncmp(cTemp, "rfio:", 5) == 0)
   {
      iMassSto = 1;
      if (iDebug)
         printf("    copy to mass storage (ArchivePool)\n");
   }
   else /* assume node:object */
   {
      iMassSto = 1;
      if (iDebug)
         printf("    copy to mass storage (ArchivePool)\n");
   }

   if (pcurAPIFile->iMassSto != iMassSto)
   {
      fprintf(fLogClient,
         "-E- %s: remote file %s incompatible with open server connection\n",
         cModule, pcFile);
      iError = -1;
      goto gEndNewFile;
   }

   if (iDebug) printf(
      "    requested server consistent with open connection (%d)\n",
      iMassSto);

   /* pcc points now to 1st '/' */
   if (iMassSto)
   {
      if (strlen(pcc) > MAX_OBJ_FS - 1)
         strncpy(pcNamefs, pcc, MAX_OBJ_FS-1);
      else
         strcpy(pcNamefs, pcc);

      pcc = pcNamefs;
      pcc++;
      pcc1 = (char *) strchr(pcc, *pcObjDelim);
      if (pcc1 == NULL)
      {
         ii = MAX_OBJ_FS-1;
         if (iDebug) printf(
            "-E- archive name starting with %s too long, max allowed %d chars\n",
            pcNamefs, ii);
         fprintf(fLogClient, 
            "-E- %s: archive name starting with %s too long, max allowed %d chars\n",
            cModule, pcNamefs, ii);
         iError = -1;
         goto gEndNewFile;
      }

      strncpy(pcc1, "\0", 1);             /* terminates archive name */
      if (strlen(pCommServ->cNamefs))
      {
         if (strcmp(pCommServ->cNamefs, pcNamefs))
         {
            fprintf(fLogClient,
               "-E- %s: invalid archive name %s, expected %s\n",
               cModule, pcNamefs, pCommServ->cNamefs);
            iError = -1;
            goto gEndNewFile;
         }
      }
      
      strcpy(pCommServ->cNamefs, pcNamefs);
      iArchLen = strlen(pcNamefs);

      if (iDebug) printf(
         "    archive name: %s (len %d chars)\n", pcNamefs, iArchLen);

      if (iArchLen <= 3)
      {
         fprintf(fLogClient,
            "-E- %s: invalid archive name '%s'\n", cModule, pcNamefs);
         iError = -1;
         goto gEndNewFile;
      }

      pcc = (char *) strchr(cTemp, *pcObjDelim);        /* full name */
      pcc++;                             /* skip '/' in archive name */
      pcc2 = (char *) strchr(pcc, *pcObjDelim);
      if (pcc2 == NULL)
      {
         fprintf(fLogClient, 
            "-E- %s: invalid remote file name %s: missing path name\n",
            cModule, pcFile);
         iError = -1;
         goto gEndNewFile;
      }

      if (iDebug)
         printf("    path + file name: %s\n", pcc2);

      pcc = pcc2;
      pcc++;                            /* skip 1st '/' in path name */
      pcc1 = (char *) strrchr(pcc, *pcObjDelim);
      if (pcc1 == NULL)
      {
         fprintf(fLogClient, 
            "-E- %s: invalid remote file name %s: missing file name\n",
            cModule, pcFile);
         iError = -1;
         goto gEndNewFile;
      }

      if (iDebug)
         printf("    file name: %s\n", pcc1);

      iPathLen = pcc2-pcc1;
      if (iPathLen < 0)
         iPathLen = -iPathLen;
      if (iDebug)
         printf("    length path name: %d\n", iPathLen);

      if (iPathLen >= MAX_OBJ_HL)
      {
         ii = MAX_OBJ_HL - 1;
         if (iDebug)
            printf("-E- path name too long, max allowed %d chars\n", ii);
         fprintf(fLogClient, 
            "-E- %s: remote path name (%d chars) too long, max allowed %d\n",
            cModule, iPathLen, ii);
         iError = -1;
         goto gEndNewFile;
      }

      strncpy(pcNamehl, pcc2, (unsigned) iPathLen);
      if (iDebug)
         printf("    path name: %s\n", pcNamehl);

      strcpy(pCommAPI->cNamehl, rawGetHLName(pcNamehl));
      if (strcmp(pcNamehl, pCommAPI->cNamehl))
      {
         strcpy(pcNamehl, pCommAPI->cNamehl);
         printf("    gStore path name corrected to %s\n", pcNamehl);

         strcpy(pcFile, cPrefix);
         strcat(pcFile, cNamefs);
         strcat(pcFile, cNamehl);
         strcat(pcFile, pcc1);
         if (iDebug) printf(
            "    new full file name: %s\n", pcFile);
      }
   
      iFileLen = strlen(pcc1);
      if (iDebug)
         printf("    length file name: %d\n", iFileLen);

      ii = MAX_OBJ_LL-1;
      if ((int) strlen(pcc1) > ii)
      {
         if (iDebug)
            printf("-E- file name too long, max allowed %d chars\n", ii);
         fprintf(fLogClient,
            "-E- %s: file name %s (%d chars) too long, max allowed %d\n",
            cModule, pcc1, iFileLen, ii);
         iError = -1;
         goto gEndNewFile;
      }

      strcpy(pcNamell, pcc1);
      if (iDebug)
         printf("    file name: %s\n", pcNamell);

   } /* (iMassSto) */
   else
   {
      strcpy(pCommServ->cNamefs, "");

      strcpy(cTemp, pcc);        /* starts now with remote path name */
      pcc = cTemp;
      pcc++;                                     /* skip leading '/' */
      pcc1 = (char *) strrchr(pcc, *pcObjDelim);
      if (pcc1 == NULL)
      {
         fprintf(fLogClient, "-E- %s: invalid remote file name %s\n",
            cModule, pcFile);
         iError = -1;
         goto gEndNewFile;
      }

      if (strlen(pcc1) > MAX_OBJ_LL-1)
      {
         ii = MAX_OBJ_LL-1;
         fprintf(fLogClient,
            "-E- %s: remote file name %s too long (max %d chars)\n",
            cModule, pcc1, ii);
         iError = -1;
         goto gEndNewFile;
      }

      strcpy(pcNamell, pcc1);
      if (iDebug) printf(
         "    remote file name on RFIO server: %s\n", pcNamell);

      strncpy(pcc1, "\0", 1);                /* terminates path name */
      if (strlen(cTemp) > MAX_OBJ_HL-1)
      {
         ii = MAX_OBJ_HL-1;
         fprintf(fLogClient,
            "-E- %s: remote path name %s too long (max %d chars)\n",
            cModule, cTemp, ii);
         iError = -1;
         goto gEndNewFile;
      }

      strcpy(pcNamehl, cTemp);
      if (iDebug) printf(
         "    remote path name on RFIO server: %s\n", pcNamehl);

   } /* (iMassSto = 0) */

   if (strlen(pCommServ->cNamehl))
   {
      if (strcmp(pCommServ->cNamehl, pcNamehl))
      {
         fprintf(fLogClient, "-W- new path name %s, previous %s\n",
            pcNamehl, pCommServ->cNamehl);
         inewPath = 1;
      }
      else
         inewPath = 0;
   }
   else
   {
      strcpy(pCommServ->cNamehl, pcNamehl);
      strcpy(pCommAPI->cNamehl, pcNamehl);
   }

   if (strlen(pCommAPI->cNamefs) < 3)
      strcpy(pCommAPI->cNamefs, pCommServ->cNamefs);
   
   strcpy(pCommAPI->cNamell, pcNamell);
   strcpy(pCommServ->cNamell, pcNamell);
   if (inewPath)
   {
      strcpy(pCommServ->cNamehl, pcNamehl);
      strcpy(pCommAPI->cNamehl, pcNamehl);
   }

   if (iDebug)
      printf("    file name: %s\n", pcNamell);

   pCommServ->iBufsizeFile = htonl(pCommAPI->iBufsizeFile);

   /********************** perform queries ***************************/

   if (iMassSto)
   {
      iATLServer = pCommAPI->iATLServer;
      if (iDebug) printf(
         "    query file %s in ATL server %d\n", pcFile, iATLServer);

      pBufQuery = &sQuery;
      pQAttr = &(sQuery.objAttr);
      
      pCommServ->iAction = htonl(QUERY_ARCHIVE_RECORD);      /* temp */

      iRC = rawQueryFile(pcurAPIFile->iSockMaster, 0,
                         pCommServ, &pBufQuery);

      pCommServ->iAction = htonl(ARCHIVE_RECORD);           /* reset */

      if (iRC)
      {
         if (iRC < 0)
         {
            if (iRC == -1001) sprintf(rfio_errmsg,
               "-E- no write access to archive %s in mass storage\n",
               pCommAPI->cNamefs);
            else if (iRC == -1000) sprintf(rfio_errmsg,
               "-E- no archive %s in mass storage found\n",
               pCommAPI->cNamefs);
            else sprintf(rfio_errmsg,
               "-E- file %s: query in mass storage failed, rc = %d\n",
               pcFile, iRC);
         }
         else sprintf(rfio_errmsg,
            "-E- file %s already available in mass storage\n",
            pcFile);
         fprintf(fLogClient, "%s", rfio_errmsg);

	 if ( (iRC == -1001) || (iRC == -1000) )
            iError = -1;
	 else
            iError = 1;

         goto gEndNewFile;
      }

      if (iDebug) printf(
         "    file %s not yet available in mass storage\n", pcFile);

   } /* (iMassSto) */

gNextCmdNewFile:
   iCopyMode = ntohl(pCopyCacheServ->iCopyMode);
   if (iCopyMode)
   {
      iCopyLen = ntohl(pCopyCacheServ->iCopyLen);
      if (iCopyLen == 0)
      {
         iCopyBuffer = 1;
         ii = irawCopyCache - HEAD_LEN;
         pCopyCacheServ->iCopyLen = htonl(ii);
         if (iDebug)
            printf("    send copy parameters to DM\n");
      }
      else if (iDebug)
         printf("    copy parameters already sent to DM\n");
   }
   else if (iDebug)
      printf("    data are only written to WC\n");

   if (iDebug)
   {
      printf("    send command buffer to server %s: id: %d, action %d, bufsize %d",
         pcurAPIFile->cNodeMover,
         pCommAPI->iIdent,
         pCommAPI->iAction,
         pCommAPI->iBufsizeFile);
      if (iCopyBuffer)
         printf(", data len %d\n",
            pCommAPI->iCommLen + irawCopyCache);
      else
         printf(", data len %d\n", pCommAPI->iCommLen);
   }

   iSockMover = pcurAPIFile->iSockMover;
   pcc = (char *) pCommServ;
   if (iCopyBuffer)
      pCommServ->iCommLen = htonl(pCommAPI->iCommLen + irawCopyCache);
                                              /* temp for data mover */

   if ( (iRC = send(iSockMover, pcc, (unsigned) irawComm, 0 )) < 0 ) 
   {
      fprintf(fLogClient, 
         "-E- %s: sending command buffer to server %s (rc = %d)\n",
         cModule, pcurAPIFile->cNodeMover, iRC); 
      perror("    "); 
      iError = -1;
      goto gEndNewFile;
   }

   if (irawComm != iRC)  
   { 
      fprintf(fLogClient,
         "-E- %s: incomplete command buffer sent to server %s (%d of %d bytes)\n",
         cModule, pcurAPIFile->cNodeMover, iRC, irawComm);

      iError = -1;
      goto gEndNewFile;
   }

   if (iDebug) printf(
      "    command buffer sent to server %s (%d byte)\n",
      pcurAPIFile->cNodeMover, irawComm);

   if (iCopyBuffer)
   {
      pcc = (char *) pCopyCacheServ;
      if ( (iRC = send(iSockMover, pcc, (unsigned) irawCopyCache, 0 )) < 0 ) 
      {
         fprintf(fLogClient, 
            "-E- %s: sending copy buffer to server %s (rc = %d)\n",
            cModule, pcurAPIFile->cNodeMover, iRC); 
         perror("    "); 
         iError = -1;
         goto gEndNewFile;
      }

      if (irawCopyCache != iRC)  
      { 
         fprintf(fLogClient,
            "-E- %s: incomplete copy buffer sent to server %s (%d of %d bytes)\n",
            cModule, pcurAPIFile->cNodeMover, iRC, irawCopyCache);

         iError = -1;
         goto gEndNewFile;
      }

      if (iDebug) printf(
         "    copy buffer sent to server %s (%d byte)\n",
         pcurAPIFile->cNodeMover, irawCopyCache);

      pCommServ->iCommLen = htonl(pCommAPI->iCommLen);
                           /* reset temp length for data mover again */
   }

   /******************* look for reply from server *******************/

   pcc = (char *) &sStatus;
   iStatusOkay = 0;
   iStatusLoop = 0;
   while (iStatusOkay == 0)
   {
      iStatusLoop++;

      memset(&sStatus, 0X00, sizeof(srawStatus));
      iRC = rawRecvStatus(iSockMover, pcc);
      iStatus = sStatus.iStatus;
      if (iDebug) printf(
         "    status %d received (%d byte)\n", iStatus, iRC);
      if (iRC != HEAD_LEN)
      {
         if (iRC < HEAD_LEN) fprintf(fLogClient,
            "-E- %s: receiving status buffer from server\n",
            cModule);
         else                              /* received error message */
         {
            if (iStatus == STA_CACHE_FULL)
            {
               if ( (iStatusLoop < iStatusLoopMax) &&
                    (iMassSto != 2) )        /* no delay for DAQPool */
               {
                  fprintf(fLogClient,
                     "-W- %s: write cache currently full ... sleep %d s (%d of %d)\n",
                     cModule, iSleepCacheFull,
                     iStatusLoop, iStatusLoopMax);
                  continue;
               }
               else
               {
                  fprintf(fLogClient,
                     "-W- %s: write cache currently full, archive to tape\n",
                     cModule);
                  pCommServ->iAction = htonl(ARCHIVE_RECORD);
                  pCommServ->iArchDev = htonl(ARCH_TAPE);
                  pCommAPI->iAction = ARCHIVE_RECORD;
                  pCommAPI->iArchDev = ARCH_TAPE;

                  /* copy from WC no longer possible */
                  iCopyMode = 0;
                  pCopyCacheServ->iCopyMode = htonl(0);

                  goto gNextCmdNewFile;
               }
            } /* (iStatus == STA_CACHE_FULL) */
            else if (iStatus == STA_SWITCH_SERVER)
            {
               strcpy(cNodeMover, sStatus.cStatus);

               fprintf(fLogClient,
                  "-W- write cache on DM %s full or too much load, switch to %s\n",
                  pcurAPIFile->cNodeMover, cNodeMover);

               if (iDebug) printf(
                  "    close connection %d to data mover %s\n",
                  iSockMover, pcurAPIFile->cNodeMover);
               iRC = shutdown(iSockMover, 2);
               if (iDebug)
                  printf("    rc shutdown = %d\n", iRC);
               
               iRC = close(iSockMover); 
               if (iDebug)
                  printf("    rc close = %d\n", iRC);
               else if (iRC == 0)
                  fprintf(fLogClient,
                     "-I- connection to data mover %s closed\n",
                     pcurAPIFile->cNodeMover);

               /*************** connect to new data mover ************/

               if (iDebug) printf(
                  "    connecting to data mover %s:%d\n",
                  cNodeMover, iPortMover);

               iRC = rconnect(cNodeMover, iPortMover,
                              &iMaxConnect, &iSockMover);
               if (iRC)
               {
                  fprintf(fLogClient, 
                     "-E- %s: cannot connect to new data mover %s:%d (ATL server %d)\n",
                     cModule, cNodeMover, iPortMover, iATLServer);
                  iError = -2;
               }

               fprintf(fLogClient, 
                 "-I- successfully connected to new data mover %s:%d (ATL server %d)",
                 cNodeMover, iPortMover, iATLServer);
               if (iMaxConnect)
                  fprintf(fLogClient, " after %ds", iMaxConnect);
               if (iDebug)
                  printf("    (socket %d)\n", iSockMover);
               else
                  fprintf(fLogClient, "\n");

               strcpy(pcurAPIFile->cNodeMover, cNodeMover);
               pcurAPIFile->iSockMover = iSockMover;

               pCopyCacheServ->iCopyLen = htonl(0);
                             /* copy buffer must be sent again to DM */
               goto gNextCmdNewFile;

            } /* (iStatus == STA_SWITCH_SERVER) */
            else
            {
               fprintf(fLogClient, "-E- message received from server:\n");
               fprintf(fLogClient, "    %s\n", sStatus.cStatus);

               if (iStatus == STA_ERROR_EOF)  
                  iError = 1;
               else
                  iError = -2;
            }

         } /* (iRC >= HEAD_LEN) */

         goto gEndNewFile;

      } /* (iRC != HEAD_LEN) */
      iStatusOkay = 1;

   } /* while (iStatusOkay == 0) */

   if (iDebug)
   {
      printf("    status (%d) received from server (%d bytes", 
             iStatus, iRC);
      if (iStatusLoop > 1)
         printf(", iter %d)\n", iStatusLoop);
      else
         printf(")\n");
   }

   iError = 0;
      /* = 1: file already in mass storage: try new name
         = -2: write cache of current data mover full: close, new open 
         = -3: write cache of all data movers full: close, retry
       */

gEndNewFile:
   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return iError;

} /* rfio_newfile */

/********************************************************************
 * rfio_close:
 *   close remote file in GSI mass storage and
 *   connection to GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_close(int iFileId)                              /* file id */
{
   char cModule[32] = "rfio_close";
   int iDebug = 0;
   int iRC;
   int iSockMaster;               /* socket for connection to master */
   int iSockMover;                 /* socket for connection to mover */
   int iStatus = STA_END_OF_SESSION;
   int ii = 0;
   char *pcc;

   srawStatus sStatusRecv;
   srawComm *pCommAPI;
   srawAPIFile *pcurAPIFile;

   if (iDebug) printf(
      "\n-D- begin %s: close remote file %d\n", cModule, iFileId);

   if (iFileCount == 0)
   {
      fprintf(fLogClient,
         "-E- %s: no remote file available for close\n", cModule);
      return -1;
   }

   while (ii < iFileMax)
   {
      if (pAPIFile[ii])
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }
      ii++;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient, "-E- %s: remote file %d cannot be closed: not found\n",
         cModule, iFileId);
      return -1;
   }

   pcurAPIFile = pAPIFile[iFileId];
   pCommAPI = &(pcurAPIFile->sCommAPI);
   iStatus = STA_END_OF_SESSION;
   iSockMaster = pcurAPIFile->iSockMaster;

   if ( (pcurAPIFile->iMassSto) &&
        (pcurAPIFile->iOpMode == 0) )               /* 'normal' RFIO */
      fprintf(fLogClient, "-I- gStore file %s%s%s closed\n", 
         pCommAPI->cNamefs, pCommAPI->cNamehl, pCommAPI->cNamell);
   
   if (iSockMaster >= 0) 
   {
      /* stop master server process */
      iRC = rawSendStatus(iSockMaster, iStatus, NULL);
      if (iRC <= 0) fprintf(fLogClient,
         "-E- %s: sending status buffer (EOS) to master\n", cModule);
      else if (iDebug) printf(
         "    status buffer (EOS) sent to master (%d bytes)\n", iRC);

      if (iDebug)
         printf("    close connection %d to master\n", iSockMaster);
      iRC = shutdown(iSockMaster, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSockMaster); 
      if (iDebug)
         printf("    rc close = %d\n", iRC);

   } /* (iSockMaster >= 0) */

   iSockMover = pcurAPIFile->iSockMover;
   if (iSockMover >= 0) 
   {
      /* stop data mover process */
      iRC = rawSendStatus(iSockMover, iStatus, NULL);
      if (iRC <= 0) fprintf(fLogClient,
         "-E- %s: sending status buffer (EOS) to data mover\n", cModule);
      else if (iDebug) printf(
        "    status buffer (EOS) sent to data mover (%d bytes)\n", iRC);

      /* receive confirmation if written to tape */
      if ( (pCommAPI->iAction == ARCHIVE_RECORD) &&
           (pCommAPI->iArchDev == ARCH_TAPE) &&
           (pcurAPIFile->iOpMode == 0) ) /* in rfio_endfile alr. done */
      {
         memset(&sStatusRecv, 0X00, sizeof(srawStatus));
         pcc = (char *) &sStatusRecv;
         iRC = rawRecvStatus(iSockMover, pcc);

         if (iRC < HEAD_LEN) fprintf(fLogClient, 
            "-E- %s: receiving confirmation of status buffer (EOS)\n",
            cModule);
         else if (iDebug) printf(
            "    status (%d) confirmed by server (%d bytes)\n",
            sStatusRecv.iStatus, iRC);
      }

      if (iDebug)
         printf("    close connection %d to data mover\n", iSockMover);
      iRC = shutdown(iSockMover, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSockMover); 
      if (iDebug)
         printf("    rc close = %d\n", iRC);
      else fprintf(fLogClient,
         "-I- connection to data mover %s closed\n",
         pcurAPIFile->cNodeMover);

   } /* (iSockMover >= 0) */

   free(pAPIFile[iFileId]);
   pAPIFile[iFileId] = NULL;
   iFileCount--;

   if (iDebug)
   {
      printf(
         "    remote file control block %d deleted (%d still active)\n",
         iFileId, iFileCount);
      printf("-D- end %s\n\n", cModule);
   }

   return 0;

} /* rfio_close */

#ifdef __USE_LARGEFILE64

/********************************************************************
 * rfio_preseek64: in remote file in GSI mass storage
 *    move read/write file mark
 *
 * created 29. 5.2007, Horst Goeringer
 ********************************************************************
 */

int rfio_preseek64(
      int iFileId,
      const struct iovec64 *piov,
      int iBufferNumber)
{
   char cModule[32] = "rfio_preseek64";
   int iDebug = 0;
   int iRC = 0;

   if (iDebug) fprintf(fLogClient,
      "\n-D- begin %s: iFileId %d\n", cModule, iFileId);

   ii = sizeof(long);
   if (ii < 8)
   {
      fprintf(fLogClient,
         "-E- 32 bit client: %s not supported\n", cModule);

      return -1;
   }

   fprintf(fLogClient, 
      "-W- %s not yet implemented for gStore\n", cModule);

   if (iDebug)
      fprintf(fLogClient, "-D- end %s\n\n", cModule);

   return iRC;
   
} /* rfio_preseek64 */

/********************************************************************
 * rfio_lseek64: in remote file in GSI mass storage
 *    move read/write file mark
 *
 * created 29. 5.2007, Horst Goeringer
 ********************************************************************
 */

int64_t rfio_lseek64(
      int iFileId,
      int64_t i64locOffset,
      int ilocSeekMode)
{
   char cModule[32] = "rfio_lseek64";
   int iDebug = 0;
   int iRC;
   int ilocOffset;
   int ii;

   ii = sizeof(long);
   if (ii < 8)
   {
      fprintf(fLogClient,
         "-E- 32 bit client: %s not supported\n", cModule);

      return -1;
   }

   if (iDebug) fprintf(fLogClient,
      "\n-D- begin %s: iFileId %d, Offset %lld, SeekMode %d\n",
      cModule, iFileId, (unsigned long) i64locOffset, ilocSeekMode);

   ilocOffset = (int) i64locOffset;

   iRC = rfio_lseek(iFileId, ilocOffset, ilocSeekMode);

   if (iDebug) fprintf(fLogClient,
      "-D- end %s: rc(rfio_lseek) = %d \n\n", cModule, iRC);

   return (int64_t) iRC;
   
} /* rfio_lseek64 */

#endif /* __USE_LARGEFILE64 */

/********************************************************************
 * rfio_lseek: in remote file in GSI mass storage
 *    move read/write file mark
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_lseek(int iFileId,                              /* file id */
               int ilocOffset,
               int ilocSeekMode)
{
   char cModule[32] = "rfio_lseek";
   int iDebug = 0;
   int iRC;
   int iSockMover;                 /* socket for connection to mover */
   int iSeekMode;
   int iOffset;

   srawAPIFile *pcurAPIFile;

   if (iDebug)
   {
      printf("\n-D- begin %s: file %d\n", cModule, iFileId);
      if (ilocSeekMode == SEEK_SET)
         printf("    position to %d bytes, \n", ilocOffset);
      else if (ilocSeekMode == SEEK_CUR)
         printf("    position to current + %d bytes, \n", ilocOffset);
      else if (ilocSeekMode == SEEK_END)
         printf("    position to file size + %d bytes, \n", ilocOffset);
      else
      {
         fprintf(fLogClient, "-E- %s: unexpected offset (%d)\n",
            cModule, ilocSeekMode);
         return -1;
      }
   }

   pcurAPIFile = pAPIFile[iFileId];
   iSeekMode = pcurAPIFile->iSeekMode;

   /* check if still seek request pending */
   if (iSeekMode != -1)
   {
      iSockMover = pcurAPIFile->iSockMover;
      iOffset = pcurAPIFile->iOffset;
      iRC = rawSendRequest(iSockMover, iSeekMode, iOffset, 0);
      if (iRC != 0)
      {
         fprintf(fLogClient, "-E- %s: sending request for seek\n", cModule);
         return -1;
      }
   }

   pcurAPIFile->iSeekMode = ilocSeekMode;
   pcurAPIFile->iOffset = ilocOffset;

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return 0;

} /* rfio_lseek */

/********************************************************************
 * rfio_fopen: open connection to gStore server and file
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

RFILE *rfio_fopen(char *pcFile,                         /* file name */
                 char *pcOptions)                         /* options */
{
   char cModule[32] = "rfio_fopen";
   int iDebug = 0;
   RFILE *fRemoteFile = NULL;
   int iCopyMode = 0;      /* ignore parameters for data stream copy */
   char *pcCopyPath = NULL;
   int iCopyFraction = 0;
   int iMaxFile = 0;
   int iPathConvention = 0;

   if (iDebug)
      printf("\n-D- begin %s: calling rfio_fopen_gsidaq\n", cModule);

   fRemoteFile = rfio_fopen_gsidaq(
      pcFile,
      pcOptions,
      iCopyMode,
      pcCopyPath,
      iCopyFraction,
      iMaxFile,
      iPathConvention);

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return fRemoteFile;

} /* rfio_fopen */

/********************************************************************
 * rfio_fopen_gsidaq_dm: open connection to gStore server and file and
 *      prepare copy to lustre or read cache before migration to tape
 *      provide DM name and running number
 *
 * created 23. 4.2010, Horst Goeringer
 ********************************************************************
 */

RFILE *rfio_fopen_gsidaq_dm(
        char *pcFile,                                  /* file name */
        char *pcOptions,                                 /* options */
        char *pcDataMover,
        int *piDataMover,
        int iCopyMode,
               /* = 0: standard RFIO, ignore following arguments
                  = 1: copy to pcCopyPath after file written to WC
                       (for high data rates,
                        don't interfere writing to cache)
                  = 2: for lustre only:
                       write each data buffer in parallel to
                       write cache and to pcCopyPath
                       (for low data rates, anyhow first buffers
                        quickly available in lustre)
                */
        char *pcCopyPath,
               /* destination where to be copied
                  = "/lustre..."  => fully qualified path name
                    if not existing: will be created according
                       to user conventions:
                       ".../d" => Hades, create .../dyydddhhmm
                  = "RC" => read cache
                */
        int iCopyFraction,
               /* = i>0: copy each ith file to pcCopyPath
                    if tape migration fails:
                       ignore iCopyFraction, copy each file
                */
        int iMaxFile,
               /* for dataFS only:
                  = 0: no file limit
                  > 0: max no. of files to be written to directory
                       max no. of files to be written to directory
                       files already existing are ignored
                       if iMaxFile reached, new dir will be created
                          (same level as previous one)
                */
        int iPathConvention)
               /* rules for creation of initial/new path
                   = 0: default convention
                     initially specified .../xxx => .../xxx
                     last .../xxx => create ...xxx1
                     last .../xxxi => create ...xxxj j=i+1
                   = 1: Hades convention: path name contains time stamp
                     initially specified .../xxx => .../xxxyydddhhmm
                     new => .../xxxyydddhhmm
                */
{
   char cModule[32] = "rfio_fopen_gsidaq_dm";
   int iDebug = 0;
   int iFileId = -1;      /* file identifier for server connection */
   int iDataMover = 0;
   int ilen;
   char *pcc;

   if (iDebug)
      printf("\n-D- begin %s: calling rfio_open_gsidaq\n", cModule);

   if (*pcOptions == 'r')
   {
      iFileId = rfio_open_gsidaq(
         pcFile, O_RDONLY, 0,
         iCopyMode, pcCopyPath, iCopyFraction, iMaxFile,
         iPathConvention);
   }
   if (*pcOptions == 'w')
   {
      iFileId = rfio_open_gsidaq(
         pcFile, O_WRONLY | O_CREAT, 0,
         iCopyMode, pcCopyPath, iCopyFraction, iMaxFile,
         iPathConvention);
   }

   if (iFileId >= 0)
   {
      ilen = strlen(pAPIFile[iFileId]->cNodeMover);
      strcpy(pcDataMover, pAPIFile[iFileId]->cNodeMover);
      if (iDebug) printf(
         "    connected with data mover %s\n", pcDataMover);

      pcc = pcDataMover;
      pcc += ilen - 2;            /* last 2 digits are the running number */
      iDataMover = atoi(pcc);
      *piDataMover = iDataMover;

      if (iDebug) printf(
         "    running number data mover: %d\n", iDataMover);
   }

   if (iDebug) printf(
      "-D- end %s: iFileId(rfio_open_gsidaq) = %d\n\n",
      cModule, iFileId);

   if (iFileId < 0)
      return NULL;

   return pAPIFile[iFileId];        /* global, filled in rfio_open */

} /* rfio_fopen_gsidaq_dm */

/********************************************************************
 * rfio_fopen_gsidaq: open connection to gStore server and file and
 *      prepare copy to lustre or read cache before migration to tape
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

RFILE *rfio_fopen_gsidaq(
        char *pcFile,                                  /* file name */
        char *pcOptions,                                 /* options */
        int iCopyMode,
               /* = 0: standard RFIO, ignore following arguments
                  = 1: copy to pcCopyPath after file written to WC
                       (for high data rates,
                        don't interfere writing to cache)
                  = 2: for lustre only:
                       write each data buffer in parallel to
                       write cache and to pcCopyPath
                       (for low data rates, anyhow first buffers
                        quickly available in lustre)
                */
        char *pcCopyPath,
               /* destination where to be copied
                  = "/lustre..."  => fully qualified path name
                    if not existing: will be created according
                       to user conventions:
                       ".../d" => Hades, create .../dyydddhhmm
                  = "RC" => read cache
                */
        int iCopyFraction,
               /* = i>0: copy each ith file to pcCopyPath
                    if tape migration fails:
                       ignore iCopyFraction, copy each file
                */
        int iMaxFile,
               /* for dataFS only:
                  = 0: no file limit
                  > 0: max no. of files to be written to directory
                       max no. of files to be written to directory
                       files already existing are ignored
                       if iMaxFile reached, new dir will be created
                          (same level as previous one)
                */
        int iPathConvention)
               /* rules for creation of initial/new path
                   = 0: default convention
                     initially specified .../xxx => .../xxx
                     last .../xxx => create ...xxx1
                     last .../xxxi => create ...xxxj j=i+1
                   = 1: Hades convention: path name contains time stamp
                     initially specified .../xxx => .../xxxyydddhhmm
                     new => .../xxxyydddhhmm
                */
{
   char cModule[32] = "rfio_fopen_gsidaq";
   int iDebug = 0;
   int iFileId = -1;      /* file identifier for server connection */

   if (iDebug)
      printf("\n-D- begin %s: calling rfio_open_gsidaq\n", cModule);

   if (*pcOptions == 'r')
   {
      iFileId = rfio_open_gsidaq(
         pcFile, O_RDONLY, 0,
         iCopyMode, pcCopyPath, iCopyFraction, iMaxFile,
         iPathConvention);
   }
   if (*pcOptions == 'w')
   {
      iFileId = rfio_open_gsidaq(
         pcFile, O_WRONLY | O_CREAT, 0,
         iCopyMode, pcCopyPath, iCopyFraction, iMaxFile,
         iPathConvention);
   }

   if (iDebug) printf(
      "-D- end %s: iFileId(rfio_open_gsidaq) = %d\n\n",
      cModule, iFileId);

   if (iFileId < 0)
      return NULL;

   return pAPIFile[iFileId];        /* global, filled in rfio_open */

} /* rfio_fopen_gsidaq */

/*********************************************************************
 * rfio_fnewfile: continue with next remote file in GSI mass storage
 *
 * created  2. 8.2004, Horst Goeringer
 *********************************************************************
 */

int rfio_fnewfile(RFILE *pRemFile,      /* remote file control block */
                   char *pcFile)                        /* file name */
{
   char cModule[32] = "rfio_fnewfile";
   int iDebug = 0;
   int iRC;
   int ii;
   int iFileId;             /* file identifier for server connection */

   if (iDebug)
   {
      printf("\n-D- begin %s\n", cModule);
      fflush(stdout);
   }

   if (iFileCount <= 0)
   {
      fprintf(fLogClient, "-E- %s: still no connection to server opened\n",
         cModule);
      return -1;
   }

   if (pRemFile == NULL)
   {
      fprintf(fLogClient, "-E- %s: invalid file ptr (NULL) specified\n", cModule);
      return -1;
   }

   if ( (pcFile == NULL) || (strlen(pcFile) < 5) )
   {
      fprintf(fLogClient, "-E- %s: invalid file '%s' specified\n",
         cModule, pcFile);
      return -1;
   }

   ii = 0;
   while (ii < iFileMax)
   {
      if (pAPIFile[ii] == pRemFile)   /* global, filled in rfio_open */
         break;
      ii++;
   }

   if (ii == iFileMax)
   {
      fprintf(fLogClient, "-E- %s: invalid file ptr specified\n", cModule);
      return -1;
   }

   iFileId = pRemFile->iFileId;
   iRC = rfio_newfile(iFileId, pcFile);

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   if (iFileId < 0)
      return -1;

   return iRC;

} /* end rfio_fnewfile */

/*********************************************************************
 * rfio_fread: read remote file in GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 *********************************************************************
 */

size_t rfio_fread(char *pcBuffer,                     /* data buffer */
                  size_t iSize,                 /* size of data item */
                  size_t iItems,             /* number of data items */
                  RFILE *pRemFile)      /* remote file control block */
{
   char cModule[32]="rfio_fread";
   int iDebug = 0;
   int iRC;
   int iFileId;           /* file identifier for server connection */
   size_t iBytes;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (pRemFile == NULL)
   {
      fprintf(fLogClient, "-E- %s: remote file not open for read\n", cModule);
      return 0;
   }

   iFileId = pRemFile->iFileId;
   iBytes = iSize*iItems;
   iRC = (int) rfio_read(iFileId, pcBuffer, iBytes);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return (unsigned) iRC;

} /* rfio_fread */

/*********************************************************************
 * rfio_fwrite: write remote file to GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 *********************************************************************
 */

size_t rfio_fwrite(const char *pcBuffer,              /* data buffer */
                   size_t iSize,                /* size of data item */
                   size_t iItems,            /* number of data items */
                   RFILE *pRemFile)     /* remote file control block */
{
   char cModule[32]="rfio_fwrite";
   int iDebug = 0;
   int iFileId;           /* file identifier for server connection */
   size_t iBytes;
   int iRC;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (pRemFile == NULL)
   {
      fprintf(fLogClient, "-E- %s: remote file not open for write\n", cModule);
      return 0;
   }

   iFileId = pRemFile->iFileId;
   iBytes = iSize*iItems;
   iRC = (int) rfio_write(iFileId, pcBuffer, iBytes);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return (unsigned) iRC;

} /* rfio_fwrite */

/********************************************************************
 * rfio_fendfile:
 *   close remote file in GSI mass storage,
 *   but keep connection to mass storage data mover
 *
 * created 16. 7.2004, Horst Goeringer
 ********************************************************************
 */

int rfio_fendfile(RFILE *pRemFile)      /* remote file control block */
{
   char cModule[32] = "rfio_fendfile";
   int iDebug = 0;
   int iFileId;           /* file identifier for server connection */
   int iRC;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   iFileId = pRemFile->iFileId;
   iRC = rfio_endfile(iFileId);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return iRC;

} /* rfio_fendfile */

/********************************************************************
 * rfio_fclose:
 *   close remote file in GSI mass storage and
 *   connection to GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_fclose(RFILE *pRemFile)      /* remote file control block */
{
   char cModule[32] = "rfio_fclose";
   int iDebug = 0;
   int iFileId;           /* file identifier for server connection */
   int iRC;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   iFileId = pRemFile->iFileId;
   iRC = rfio_close(iFileId);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return iRC;

} /* rfio_fclose */

/********************************************************************
 * rfio_access: for remote file in GSI mass storage
 *   get access status
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_access(const char *pcFile,                   /* file name */
                int iMode)
{
   char cModule[32] = "rfio_access";
   int iDebug = 0;
   int iRC = 0;

   int iFileId = -1;
   
   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   /* directories not (yet) supported */
   if (iMode == X_OK)
   {
      if (iDebug)
         printf("    check if executable - not supported\n");
      iRC = 1;
      goto gEndAccess;
   }
   
   if (iMode == F_OK)
   {
      if (iDebug)
         printf("    check for existence\n");

      iFileId = rfio_open(pcFile, O_RDONLY, -1);
      if (iFileId >= 0)
      {
         iRC = rfio_close(iFileId);
         if (iRC)
         {
            printf("-E- closing remote file %s\n", pcFile);
            perror("    ");
         }
         else if (iDebug)
            printf("-D- remote file %s closed\n", pcFile);

         /* file exists, access rights already checked */

         /* overwrite not allowed */
         if (iMode == W_OK)
         {
            if (iDebug)
               printf("    file %s must not be overwritten\n",
                  pcFile);
            iRC = 1;
         }
         else if (iMode == R_OK)
         {
            if (iDebug)
               printf("    file %s is readable\n", pcFile);
            iRC = 0;
         }
         else
         {
            if (iDebug)
               printf("    file %s exists\n", pcFile);
            iRC = 0;
         }
      }
      else
      {
         /* file not existing */
         if (iMode == W_OK)
         {
            if (iDebug)
               printf("    file %s may be written\n", pcFile);
            iRC = 0;
         }
         else
         {
            if (iDebug)
               printf("    file %s not existing\n", pcFile);
            iRC = 1;
         }
      } /* file not existing */
   } /* check if file existing */

   /* assume correct handling of caller (root) after check for existence */
   if (iMode == R_OK)
   {
      if (iDebug)
         printf("    file %s is readable\n", pcFile);
      iRC = 0;
   }
   if (iMode == W_OK)
   {
      if (iDebug)
         printf("    file %s may be written\n", pcFile);
       iRC = 0;
   }

gEndAccess:
   if (iDebug)
      printf("-D- end %s\n", cModule);

   return iRC;

} /* rfio_access */

#ifdef __USE_LARGEFILE64

/*********************************************************************
 * rfio_fstat64: for remote file in GSI mass storage
 *   get file status
 *
 * created 29. 5.2007, Horst Goeringer
 *********************************************************************
 */

int rfio_fstat64(int iFileId,
                 struct stat64 *pStatBuf64)/* file statistics buffer */
{
   char cModule[32] = "rfio_fstat64";
   int iDebug = 0;
   int iRC = 0;
   int ii;

   unsigned long *plFileSizeC;          /* ptr to file size in sComm */
   srawComm *pComm;

   if (iDebug)
   {
      fprintf(fLogClient, "\n-D- begin %s: file %d\n", cModule, iFileId);
      if (iFileCount)
         fprintf(fLogClient, ", %d remote files currently open\n", iFileCount);
      else
         fprintf(fLogClient, "\n");
   }

   ii = sizeof(long);
   if (ii < 8)
   {
      fprintf(fLogClient,
         "-E- 32 bit client: %s not supported\n", cModule);

      return -1;
   }

   ii = 0;
   if (iFileCount > 0) while (ii <= iFileMax)
   {
      if (pAPIFile[ii] != NULL)
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }

      ii++;
   }

   if ( (ii > iFileMax) || (iFileCount == 0) )
   {
      fprintf(fLogClient, "-E- %s: specified remote fileId %d not found\n",
         cModule, iFileId);
      if (iDebug)
         fprintf(fLogClient, "-D- end %s\n\n", cModule);

      return -1;
   }

   pComm = &(pAPIFile[ii]->sCommAPI);
   plFileSizeC = (unsigned long *) &(pComm->iFileSize);

   if (iDebug)
   {
      fprintf(fLogClient, "    specified remote fileId %d found:\n", iFileId);
      fprintf(fLogClient, "    object %s%s%s, filesize %lu byte\n",
         pComm->cNamefs, pComm->cNamehl, pComm->cNamell, *plFileSizeC);
   }

   /* initialize stat structure with parameters of local stream stdin */
   iRC = fstat64(0, pStatBuf64);
   if (iRC)
   {
      fprintf(fLogClient, "-E- %s: fstat64() failed, rc = %d\n", cModule, iRC);
      if (iDebug)
         printf("-D- end %s\n\n", cModule);

      return -1;
   }
 
   /* now set correct file size */
   pStatBuf64->st_size = *plFileSizeC;

   if (iDebug) fprintf(fLogClient,
      "-D- end %s: irc(fstat64) = %d\n", cModule, iRC);

   return iRC;

} /* rfio_fstat64 */

#endif /* __USE_LARGEFILE64 */

/*********************************************************************
 * rfio_fstat: for remote file in GSI mass storage
 *   get file status
 *
 * created 26. 2.2001, Horst Goeringer
 *********************************************************************
 */

int rfio_fstat(int iFileId,                               /* file id */
               struct stat *pStatBuf) /* buffer with file statistics */
{
   char cModule[32] = "rfio_fstat";
   int iDebug = 0;
   int iRC = 0;
   int ii = 0;

   srawComm *pComm;

   if (iDebug)
   {
      printf("\n-D- begin %s", cModule);
      if (iFileCount)
         printf(", %d remote files currently open\n", iFileCount);
      else
         printf("\n");
   }

   ii = 0;
   if (iFileCount > 0) while (ii <= iFileMax)
   {
      if (pAPIFile[ii] != NULL)
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }

      ii++;
   }

   if ( (ii > iFileMax) || (iFileCount == 0) )
   {
      fprintf(fLogClient, "-E- %s: specified remote fileId %d not found\n",
         cModule, iFileId);
      if (iDebug)
         printf("-D- end %s\n\n", cModule);

      return -1;
   }

   pComm = &(pAPIFile[ii]->sCommAPI);
   if (iDebug)
   {
      printf("    specified remote fileId %d found:\n", iFileId);
      printf("    object %s%s%s, filesize %u byte\n",
         pComm->cNamefs, pComm->cNamehl, pComm->cNamell,
         pComm->iFileSize);
   }

   /* bzero(pStatBuf, sizeof(pStatBuf)); funktioniert nicht als init */

   /* initialize stat structure with parameters of local stream stdin */
   iRC = fstat(0, pStatBuf);
   if (iRC)
   {
      fprintf(fLogClient, "-E- %s: fstat() failed, rc = %d\n", cModule, iRC);
      if (iDebug)
         printf("-D- end %s\n\n", cModule);

      return -1;
   }
 
   /* now set correct file size */
   pStatBuf->st_size = pComm->iFileSize;

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return 0;

} /* rfio_fstat */

/********************************************************************
 * rfio_parse: for remote file in GSI mass storage
 *   parse name to host and path
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_parse(char *pcFile,                           /* file name */
               char **pcNode,
               char **pcPath)
{
   char cModule[32] = "rfio_parse";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 0;

} /* rfio_parse */

/********************************************************************
 * rfio_perror: for remote file in GSI mass storage
 *   print error message to stderr
 *
 * created 26. 2.2001, Horst Goeringer
 ********************************************************************
 */

void rfio_perror(const char *pUMsg)  /* user prefix to error message */
{
   char cModule[32] = "rfio_perror";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (strlen(rfio_errmsg) == 0)
      fprintf(stderr, "%s : No error message\n", pUMsg);
   else
      fprintf(stderr, "%s : %s\n", pUMsg, rfio_errmsg);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return;

} /* rfio_perror */

/********************************************************************
 * rfio_serror: for remote file in GSI mass storage
 *   print last RFIO related error message to string
 *
 * created 26. 2.2001, Horst Goeringer
 ********************************************************************
 */

char * rfio_serror()
{
   char cModule[32] = "rfio_serror";
   int iDebug = 0;

   if (iDebug) printf(
      "\n-D- begin %s: error (len %d byte):\n",
      cModule, (int) strlen(rfio_errmsg));

   printf(rfio_errmsg);
   fprintf(fLogClient, rfio_errmsg);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   if (strlen(rfio_errmsg) == 0)
      return NULL;

   return rfio_errmsg;

} /* rfio_serror */

/********************************************************************
 * rfio_unlink: for remote file in GSI mass storage
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_unlink(const char *pcFile)                   /* file name */
{
   char cModule[32] = "rfio_unlink";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   fprintf(fLogClient, 
      "-W- %s not yet implemented for gStore\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 1;

} /* rfio_unlink */

/********************************************************************
 * rfiosetopt: for remote file in GSI mass storage
 *  
 * created  9. 9.2002, Horst Goeringer
 ********************************************************************
 */

int rfiosetopt(int iopt, int *pival, int ilen)
{
   char cModule[32] = "rfiosetopt";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 0;

} /* rfiosetopt */

/********************************************************************
 * rfio_mkdir: for remote file in GSI mass storage
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

int rfio_mkdir(const char *path, int mode)
{
   char cModule[32] = "rfio_mkdir";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   fprintf(fLogClient, 
      "-W- %s not yet implemented for gStore\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 0;

} /* rfio_mkdir */

/********************************************************************
 * rfio_opendir: for remote file in GSI mass storage
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

void *rfio_opendir(const char *dirpath)
{
   char cModule[32] = "rfio_opendir";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   fprintf(fLogClient, 
      "-W- %s not yet implemented for gStore\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 0;

} /* rfio_opendir */

/********************************************************************
 * rfio_readdir: for remote file in GSI mass storage
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

void *rfio_readdir(void *dirp)
{
   char cModule[32] = "rfio_readdir";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   fprintf(fLogClient, 
      "-W- %s not yet implemented for gStore\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 0;

} /* rfio_readdir */

/********************************************************************
 * rfio_closedir: for remote file in GSI mass storage
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

int rfio_closedir(void *dirp)
{
   char cModule[32] = "rfio_closedir";
   int iDebug = 0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   fprintf(fLogClient, 
      "-W- %s not yet implemented for gStore\n", cModule);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return 0;

} /* rfio_closedir */

#ifdef __USE_LARGEFILE64

/********************************************************************
 * rfio_stat64: for remote file in GSI mass storage
 *   get file status
 *
 * created 29. 5.2007, Horst Goeringer
 ********************************************************************
 */

int rfio_stat64(const char *pcFile, struct stat64 *pStatBuf64)
{
   char cModule[32] = "rfio_stat64";
   int iDebug = 0;
   int iRC;

   int iMapFound = 0;             /* =1: specified file already open */
   int iFileId = -1;
   int iRC;
   int ii;
   char *pcc;
   char cObject[MAX_FULL_FILE] = "";

   srawComm *pComm;
   unsigned long *plFileSizeC;          /* ptr to file size in sComm */

   if (iDebug)
      fprintf(fLogClient, "\n-D- begin %s: file %s\n", cModule, pcFile);

   ii = sizeof(long);
   if (ii < 8)
   {
      fprintf(fLogClient,
         "-E- 32 bit client: %s not supported\n", cModule);

      return -1;
   }

   iOnlyQuery = 1;                                /* no DM selection */
   ii = 0;
   if (iFileCount > 0)
      while (ii <= iFileMax)
   {
      if (pAPIFile[ii] != NULL)
      {
         pComm = &(pAPIFile[ii]->sCommAPI);
         strcpy(cObject, pComm->cNamefs);
         strcat(cObject, pComm->cNamehl);
         strcat(cObject, pComm->cNamell);

         if (iDebug) fprintf(fLogClient,
            "    %d: %s already open\n", ii, cObject);

         pcc = strstr(pcFile, cObject);
         if (pcc != NULL)
         {
            iMapFound = 1;
            if (iDebug)
               fprintf(fLogClient, "    %d: %s maps\n", ii, cObject);
            break;
         }
      }
      else if (iDebug) fprintf(fLogClient,
         "    %d: no (more) open file found\n", ii);

      ii++;
   }

   if (iMapFound == 0)
   {
      if (iDebug) fprintf(fLogClient,
         "    remote file %s not yet open\n", pcFile);

      iFileId = rfio_open( (char *) pcFile, O_RDONLY, -1);
      if (iFileId < 0) fprintf(fLogClient,
         "-E- %s: open for remote file %s failed\n", cModule, pcFile);
   }
   else
   {
      iFileId = pAPIFile[ii]->iFileId;
      if (iFileId < 0) fprintf(fLogClient,
         "-E- %s: invalid handle (%d) for remote file %s found\n",
         cModule, iFileId, pcFile);
   }

   /* info still available, must be freed here */
   if (iFileId >= 0)
   {
      pComm = &(pAPIFile[ii]->sCommAPI);
      plFileSizeC = (unsigned long *) &(pComm->iFileSize);

      if (iDebug)
      {
         fprintf(fLogClient, "    remote fileId %d:\n", iFileId);
         fprintf(fLogClient, "    object %s%s%s, filesize %lu byte\n",
            pComm->cNamefs, pComm->cNamehl, pComm->cNamell, 
            *plFileSizeC);
      }

      /* for caller */
      pStatBuf64->st_size = *plFileSizeC;

      free(pAPIFile[iFileId]);
      pAPIFile[iFileId] = NULL;
      iFileCount--;
   }


   if (iDebug)
      fprintf(fLogClient, "-D- end %s\n\n", cModule);

   return iRC;

} /* rfio_stat64 */

#endif /* __USE_LARGEFILE64 */

/********************************************************************
 * rfio_stat: for remote file in GSI mass storage
 *   get file status
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

int rfio_stat(const char *pcFile, struct stat *pStatBuf)
{
   char cModule[32] = "rfio_stat";
   int iDebug = 0;

   int iMapFound = 0;             /* =1: specified file already open */
   int iFileId = -1;
   int iRC;
   int ii;
   char *pcc;
   char cObject[MAX_FULL_FILE] = "";

   srawComm *pComm;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   iOnlyQuery = 1;                                /* no DM selection */
   ii = 0;
   if (iFileCount > 0)
      while (ii <= iFileMax)
   {
      if (pAPIFile[ii] != NULL)
      {
         pComm = &(pAPIFile[ii]->sCommAPI);
         strcpy(cObject, pComm->cNamefs);
         strcat(cObject, pComm->cNamehl);
         strcat(cObject, pComm->cNamell);

         if (iDebug)
            printf("    %d: %s already open\n", ii, cObject);

         pcc = strstr(pcFile, cObject);
         if (pcc != NULL)
         {
            iMapFound = 1;
            if (iDebug)
               printf("    %d: %s maps\n", ii, cObject);
            break;
         }
      }
      else if (iDebug)
         printf("    %d: no (more) open file found\n", ii);

      ii++;
   }

   if (iMapFound == 0)
   {
      if (iDebug) printf(
         "    remote file %s not yet open\n", pcFile);

      iFileId = rfio_open( (char *) pcFile, O_RDONLY, -1);
      if (iFileId < 0) fprintf(fLogClient,
         "-E- %s: open for remote file %s failed\n", cModule, pcFile);
   }
   else
   {
      iFileId = pAPIFile[ii]->iFileId;
      if (iFileId < 0) fprintf(fLogClient,
         "-E- %s: invalid handle (%d) for remote file %s found\n",
         cModule, iFileId, pcFile);
   }

   /* info still available, must be freed here */
   if (iFileId >= 0)
   {
      pComm = &(pAPIFile[ii]->sCommAPI);
      if (iDebug)
      {
         printf("    remote fileId %d:\n", iFileId);
         printf("    object %s%s%s, filesize %ld byte\n",
            pComm->cNamefs, pComm->cNamehl, pComm->cNamell, 
            pComm->iFileSize);
      }

      /* for caller */
      pStatBuf->st_size = pComm->iFileSize;

      free(pAPIFile[iFileId]);
      pAPIFile[iFileId] = NULL;
      iFileCount--;
   }

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   if (iFileId >= 0)
      return 0;
   else
      return iFileId;

} /* rfio_stat */

/********************************************************************
 * rfio_cache_stat: for remote file in GSI mass storage
 *   returns cache status of file:
 *   < 0: not in gStore or error
 *   = 0: in offline storage, staging before access strongly recommended
 *   > 0: available for fast access
 *      = 1: in read cache (staged)
 *      = 2: in write cache, staging before access recommended
 *           (users do not know when migration to offline storage
 *            takes place)
 *
 * created 12.11.2008, Horst Goeringer
 ********************************************************************
 */

int rfio_cache_stat(const char *pcFile)
{
   char cModule[32] = "rfio_cache_stat";
   int iDebug = 0;

   int iMapFound = 0;             /* =1: specified file already open */
   int iFileId = -1;
   int iRC;
   int iCache = 0;                                 /* to be returned */
   int ii;
   char *pcc;
   char cObject[MAX_FULL_FILE] = "";

   srawComm *pComm;

   if (iDebug)
   {
      printf("\n-D- begin %s: %d files open\n", cModule, iFileCount);
      printf("    get cache status of file %s\n", pcFile);
   }

   iOnlyQuery = 1;                                /* no DM selection */
   ii = 0;
   if (iFileCount > 0)
      while (ii <= iFileMax)
   {
      if (pAPIFile[ii] != NULL)
      {
         pComm = &(pAPIFile[ii]->sCommAPI);
         strcpy(cObject, pComm->cNamefs);
         strcat(cObject, pComm->cNamehl);
         strcat(cObject, pComm->cNamell);

         if (iDebug)
            printf("    %d: %s already open\n", ii, cObject);

         pcc = strstr(pcFile, cObject);
         if (pcc != NULL)
         {
            iMapFound = 1;
            if (iDebug)
               printf("    %d: %s maps\n", ii, cObject);
            break;
         }
      }
      else if (iDebug)
         printf("    %d: no (more) open file found\n", ii);

      ii++;
   }

   if (iMapFound == 0)
   {
      if (iDebug) printf(
         "    remote file %s not yet open\n", pcFile);

      iFileId = rfio_open( (char *) pcFile, O_RDONLY, -1);
      if (iFileId < 0)
      {
         fprintf(fLogClient,
            "-E- remote file %s not in gStore or unavailable\n",
            pcFile);

         iCache = -1;
         goto gEndCacheStat;
      }

      if (iDebug) printf(
         "    remote file %s successfully opened for query and closed\n", pcFile);
   }
   else
   {
      iFileId = pAPIFile[ii]->iFileId;
      if (iFileId < 0)
      {
         fprintf(fLogClient,
            "-E- %s: invalid handle (%d) for remote file %s found\n",
            cModule, iFileId, pcFile);

         iCache = -1;
         goto gEndCacheStat;
      }
   }

   /* info still available, must be freed here */
   if (iFileId >= 0)
   {
      pComm = &(pAPIFile[ii]->sCommAPI);
      if (pComm->iStageFSid > 0)
         iCache = 1;                           /* file in read cache */ 
      else if (pComm->iFSidWC > 0)
         iCache = 2;                          /* file in write cache */ 
      else
         iCache = 0;                            /* file not in cache */

      if (iDebug)
      {
         printf("    remote fileId %d:\n", iFileId);
         printf("    object %s%s%s, size %ld byte, cache status %d\n",
            pComm->cNamefs, pComm->cNamehl, pComm->cNamell, 
            pComm->iFileSize, iCache);
         if (pComm->iStageFSid > 0)
            printf("    read cache FS %d\n", pComm->iStageFSid);
         else if (pComm->iFSidWC > 0)
            printf("    write cache FS %d\n", pComm->iFSidWC);
      }

      free(pAPIFile[iFileId]);
      pAPIFile[iFileId] = NULL;
      iFileCount--;
   }

gEndCacheStat:
   iOnlyQuery = 0;
   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return iCache;

} /* rfio_cache_stat */

/********************************************************************
 * rfio_gsi_query: query file information of opened remote file
 *
 * created  7. 2.2001, Horst Goeringer
 ********************************************************************
 */

int rfio_gsi_query(int iFileId,                          /* file id */
                   int iFull,                          /* verbosity */
                   int iOut,            /* max length output string */
                   char *pInfo)                      /* info string */
{
   char cModule[32] = "rfio_gsi_query";
   int iDebug = 0;
   int iRC = 0;
   int ii = 0;

   srawAPIFile *pcurAPIFile;
   srawObjAttr *pQueryAPI;                      /* query information */

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (iFileCount == 0)
   {
      printf("-E- %s: no remote file available\n", cModule);
      iRC = -1;
      goto gEndQuery;
   }

   while (ii < iFileMax)
   {
      if (pAPIFile[ii])
      {
         if (pAPIFile[ii]->iFileId == iFileId)
            break;
      }
      ii++;
   }

   if (ii == iFileMax)
   {
      printf("-E- %s: remote file %d not found\n", cModule, iFileId);
      iRC = -1;
      goto gEndQuery;
   }

   pcurAPIFile = pAPIFile[iFileId];
   pQueryAPI = &(pcurAPIFile->sQueryAPI);
   iRC = rawQueryString(pQueryAPI, iFull, iOut, pInfo);
   if (iDebug)
   {
      printf("    %s\n", pInfo);
      if (iRC != iOut) printf(
         "    (used %d byte, avail %d byte)\n", iRC, iOut);
   }

gEndQuery:
   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return iRC;

} /* rfio_gsi_query */

