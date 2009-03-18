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
 *   new version
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
 *********************************************************************
 * */

static int iPortMaster = PORT_MASTER;   /* port number master server */
static int iPortMover = PORT_MOVER;        /* port number data mover */
static int iMB = 1024000;                                    /* 1 MB */

/* definitions for command interface */
#define MAX_CMDPARM 20           /* max number of command parameters */
#define MAX_CMDPARM_LEN 256      /* max length of command parameters */

typedef struct                  /* list of tape file names and sizes */
{
   char cFileName[MAX_TAPE_FILE+1];
   unsigned long iFileSize;
   long iBufferSize;
   void *pNext;
} srawTapeFileList;

/* communication buffer passed to client action routines */
typedef struct
{
   int iAction;                 /* client action according to ACTION */
   int iSocket;                /* socket id for connection to server */
   int iPort;                           /* port number master server */
   char cNode[MAX_NODE];        /* node name requested master server */
   int iReqMaster;                 /* no. of requested master server */
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
   int iAltQuery;                          /* query alternate master */
   int iMaxQuery;                   /* max no. of queries per buffer */
   int iFilesIgnore;                  /* 1st files in list to ignore */
   char cMsg[STATUS_LEN];                /* message in case of error */
} srawCliActionComm;

typedef struct                                 /* info for file list */
{
   char cFile[MAX_FILE];            /* full file name on client side */
} srawArchList;

typedef struct                           /* info for TSM object list */
{
   char cNamell[MAX_OBJ_LL];                /* object low level name */
   unsigned int iObjHigh;              /* upper four bytes object Id */
   unsigned int iObjLow;               /* lower four bytes object Id */
   unsigned int iRestoHigh;        /* upper four bytes restore order */
   unsigned int iRestoLow;         /* lower four bytes restore order */
   int iFileType;                     /* fixed record or stream file */
   int iBufsizeFile;                                /* record length */
   unsigned long iFileSize;                 /* size of file in bytes */
   int iMaster;   /* running no. of master server (historical order) */
   int iStageFS;                /* > 0: in StagePool, no. of StageFS */
   char cNodeMover[MAX_NODE];      /* node name data mover if staged */
   int iArchiveFS;          /* > 0: in ArchivePool, no. of ArchiveFS */
   char cArchiveDate[MAX_DATE];      /* creation date in ArchivePool */
   char cOwner[MAX_OWNER];         /* account name of archiving user */
} srawRetrList;

typedef struct                             /* API file control block */
{
   int iFileId;         /* API file identifier for server connection */
   int iFlags;                                          /* from open */
   int iMode;                                           /* from open */
   char cNodeMaster[MAX_NODE];         /* node name of master server */
   int iSockMaster;               /* socket for connection to master */
   char cNodeMover[MAX_NODE];             /* node name of data mover */
   int iSockMover;                 /* socket for connection to mover */
   int iSeekMode;
   int iOffset;
   int iBufsizeAct;
   int iBufsizeServ;                                 /* not yet used */
   int iBufsizeServD;                                /* not yet used */
   int iBufnoServ;       /* no. of buffers transfered from/to server */
   int iDataServ;          /* no. of bytes transfered from/to server */
   srawComm sCommAPI;     /* communication buffer for API procedures */
   srawComm sCommServ;              /* copy for server in net format */
   srawObjAttr sQueryAPI;                       /* query information */
} srawAPIFile;

/* client operating system */
static char *pcOS;
#ifdef _AIX
static char cOS[MAX_OS] = "AIX4   ";
#endif
#ifdef Linux
static char cOS[MAX_OS] = "Linux2 ";
#endif
#ifdef Lynx
static char cOS[MAX_OS] = "Lynx   ";
#endif
#ifdef VMS
static char cOS[MAX_OS] = "VMS7   ";
#endif
#ifdef WIN32
static char cOS[MAX_OS] = "WIN32   ";
#endif


