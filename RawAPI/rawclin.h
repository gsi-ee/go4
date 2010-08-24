/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created  1. 3.1996 by Horst Goeringer
 *********************************************************************
 * rawclin.h
 *   structures and definitions needed for rawserv clients only
 *********************************************************************
 *  3. 2.1998, H.G.: add iFileSize to srawRetrList
 * 17. 1.2001, H.G.: add srawAPIFile for API clients
 * 22. 1.2001, H.G.: pad OS names with trailing blanks
 *  7. 2.2001, H.G.: in srawAPIFile: srawObjAttr added
 * 18. 7.2001, H.G.: add mover node name to srawRetrList
 * 24. 8.2001, H.G.: add stage flag to srawRetrList
 *  5. 8.2002, H.G.: srawAPIClient: replace iSockServ by iSockMaster
 *                   and iSockMover, add cNodeMover
 * 14.10.2002, H.G.: ported to Lynx
 * 27. 1.2003, H.G.: structure srawCliActionComm added
 *  3. 2.2003, H.G.: add master number to srawRetrList
 * 14. 2.2003, H.G.: srawCliActionComm: iDebug -> iReqMaster
 * 17. 2.2003, H.G.: srawCliActionComm: add tape infos
 *                   new statics iPortMaster, iPortMover
 * 18. 2.2003, H.G.: rename sTapeFileList->srawTapeFileList
 * 10. 4.2003, H.G.: add iFileType to srawRetrList
 * 11. 6.2003, H.G.: srawCliActionComm: new iAltQuery, iMaxQuery
 *  5.12.2003, H.G.: srawCliActionComm: new iFilesIgnore
 * 23. 2.2004, H.G.: srawCliActionComm: new iObjArchiveDB, 
 *                      iObjStaged -> iObjStageDB
 *                   srawRetrList: new iArchiveFS, iStage -> iStageFS
 *  4. 3.2004, H.G.: srawRetrList: new cArchiveDate, cOwner
 * 20. 8.2004, H.G.: srawAPIFile: new iAction, iBufsizeServD->iMassSto
 * 15.12.2004, H.G.: srawAPIFile: iBufsizeServ->iOpMode
 * 13. 3.2006, H.G.: declare iPortMaster, iPortMover in sources
 *  9.10.2006, H.G.: srawCliActionComm: new iATLServer
 *  8. 2.2007, H.G.: srawRetrList: iMaster -> iATLServer
 * 19. 3.2008, H.G.: srawRetrList: cNodeMover -> cMoverStage,
 *                   iArchiveFS -> iCacheFS
 *                   new: cMoverCache, iRestoHighHigh, iRestoHighLow,
 *                   iRestoLowHigh
 *  8. 4.2008, H.G.: declare and set cPathPrefix,cDataFSHigh 
 *                   declare iPathPrefix, iDataFSHigh
 *  8. 5.2008, H.G.: move static declaration [p]cOS -> rawcommn.h
 *  9. 5.2008, H.G.: remove srawArchList
 *                   (replaced by srawFileList in rawcommn.h)
 *  2. 6.2008, H.G.: replace long->int if 64bit client (ifdef SYSTEM64)
 *                   server still 32 bit
 *  3. 9.2008, H.G.: move cDataFSHigh, iDataFSHigh -> rawcommn.h
 * 16.10.2008, H.G.: srawRetrList: add iPoolId
 * 14.11.2008, H.G.: indicate display mode in  srawAPIFile.iOpMode
 * 12. 2.2009, H.G.: add structure srawCopyCache to srawAPIFile
 * 13.11.2009, H.G.: srawRetrList: enhance for recursive/parallel stage:
 *                   add cNamehl, iFileSize2, iFileSet
 *                   enlarge cNamell (MAX_OBJ_LL -> MAX_OBJ_LL_N) and
 *                   cNamehl (MAX_OBJ_HL -> MAX_OBJ_HL_N)
 * 18.12.2009, H.G.: MAX_OBJ_HL_N -> MAX_OBJ_HL, 
 *                   MAX_OBJ_LL_N -> MAX_OBJ_LL
 *********************************************************************
 * */

static int iMB = 1024000;                                    /* 1 MB */
static char cPathPrefix[16] = "@PATH:";              /* for filelist */
static int iPathPrefix = 0;        /* >0: path for files in filelist */

/* definitions for command interface */
#define MAX_CMDPARM 20           /* max number of command parameters */
#define MAX_CMDPARM_LEN 256      /* max length of command parameters */

typedef struct                  /* list of tape file names and sizes */
{
   char cFileName[MAX_TAPE_FILE+1];
#ifdef SYSTEM64                     /* 64 bit client - 32 bit server */
   unsigned int iFileSize;
   int iBufferSize;
#else                                        /* 32 bit client/server */
   unsigned long iFileSize;
   long iBufferSize;
#endif
   void *pNext;
} srawTapeFileList;

/* communication buffer passed to client action routines */
typedef struct
{
   int iAction;                 /* client action according to ACTION */
   int iSocket;                /* socket id for connection to server */
   int iPort;                            /* port number entry server */
   char cNode[MAX_NODE];         /* node name requested entry server */
   int iReqMaster;                  /* used for requested ATL server */
   srawComm sComm;          /* communication structure client/server */
   int iTapeFile; /* =0: disk file, =1: tape file, =2: tape filelist */
   int iScanTape;              /* =1: scan input tape before archive */
   char *pcTape;                         /* logical tape volume name */
   char *pcFile;              /* specified file name on disk or tape */
   int *piBuffer;                          /* ptr to query data area */
   int *piDataMover;                       /* ptr to data mover list */
   int *piObjBuf;            /* buffer containing object list of TSM */
   int *piFileList;/* buffer containing list of files to be archived */
   int iBuffers;                 /* no. of filelist buffers in chain */
   int iObjSelect;                    /* no. of TSM objects selected */
   int iObjStageDB;               /* no. of TSM objects in StagePool */
   int iObjArchiveDB;               /* no. of objects in ArchivePool */
   int iPurge;           /* no. of files to be removed from filelist */
   int iStatus;                         /* output: status for caller */
   int iAltQuery;         /* query alternate entry server (not used) */
   int iMaxQuery;                   /* max no. of queries per buffer */
   int iFilesIgnore;                  /* 1st files in list to ignore */
   int iATLServer;                /* =1 aixtsm1, =2 gsitsma, =0 both */
   char cMsg[STATUS_LEN];                /* message in case of error */
} srawCliActionComm;

typedef struct                           /* info for TSM object list */
{
   int iFileSet;               /* no. of file set (handled parallel) */
   char cNamehl[MAX_OBJ_HL];               /* object high level name */
   char cNamell[MAX_OBJ_LL];                /* object low level name */
   unsigned int iObjHigh;              /* upper four bytes object Id */
   unsigned int iObjLow;               /* lower four bytes object Id */
   unsigned int iRestoHigh;          /* top four bytes restore order */
   unsigned int iRestoHighHigh;    /* hi_hi four bytes restore order */
   unsigned int iRestoHighLow;     /* hi_lo four bytes restore order */
   unsigned int iRestoLowHigh;     /* lo_hi four bytes restore order */
   unsigned int iRestoLow;         /* lo_lo four bytes restore order */
   int iFileType;                     /* fixed record or stream file */
   int iBufsizeFile;                                /* record length */
#ifdef SYSTEM64                     /* 64 bit client - 32 bit server */
   unsigned long iFileSize;           /* 64 bit file size (in bytes) */
#else                                        /* 32 bit client/server */
   unsigned long iFileSize;           /* 32 bit file size (in bytes) */
   unsigned long iFileSize2; /* dummy for unique length in 32/64 bit */
#endif
   int iATLServer;           /* ATL server: =1: aixtsm1, =2: gsitsma */
   int iPoolId;                              /* disk pool identifier */
   char cMoverStage[MAX_NODE];     /* node name data mover if staged */
   int iStageFS;                /* > 0: in StagePool, no. of StageFS */
   char cMoverCache[MAX_NODE];  /* name data mover if in write cache */
   int iCacheFS;         /* > 0: in write cache, no. of writeCacheFS */
   char cArchiveDate[MAX_DATE];      /* creation date in ArchivePool */
   char cOwner[MAX_OWNER];         /* account name of archiving user */
} srawRetrList;

typedef struct                             /* API file control block */
{
   int iFileId;         /* API file identifier for server connection */
   int iFlags;                                          /* from open */
   int iMode;                                           /* from open */
   char cNodeMaster[MAX_NODE];          /* node name of entry server */
   int iSockMaster;         /* socket for connection to entry server */
   char cNodeMover[MAX_NODE];             /* node name of data mover */
   int iSockMover;                 /* socket for connection to mover */
   int iSeekMode;
   int iOffset;
   int iBufsizeAct;
   int iOpMode;
          /* =0: 'normal' RFIO mode
             =1: use rfio_endfile/rfio_newfile (GSI extension)
             =2: display only, no remote file open (GSI extension)   */
   int iAction;          /* =1: read from server, =2 write to server */
   int iMassSto;    /* =1: connect to GSI mass storage (ArchivePool)
                       =2: connect to GSI mass storage (DAQPool)
                       =0: connect to RFIO read/write server         */
   int iBufnoServ;       /* no. of buffers transfered from/to server */
   int iDataServ;          /* no. of bytes transfered from/to server */
   srawComm sCommAPI;     /* communication buffer for API procedures */
   srawComm sCommServ;              /* copy for server in net format */
   srawObjAttr sQueryAPI;                       /* query information */
   srawCopyCache sCopyCacheServ;
                       /* info on DAQ data stream copy in net format */
} srawAPIFile;

