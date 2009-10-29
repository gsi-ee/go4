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

/****************************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 25. 1.1996 by Horst Goeringer
 ****************************************************************************
 * rawcommn.h
 *    communication structure tsmcli client - servers
 ****************************************************************************
 * 11. 9.1996, H.G.: alternate archive tape device "ARCH_TAPE_2" added
 * 26. 9.1996, H.G.: new archive tape device "ARCH_TAPE_MC" added
 *                 "ARCH_TAPE_2" changed to "ARCH_TAPE_MC_2"
 *  8. 1.1997, H.G.: new parameter MAX_FILE_SIZE
 * 21. 7.1997, H.G.: new parameter srawComm.cliNode
 * 10. 9.1997, H.G.: new ACTION value STAGE
 * 15. 9.1997, H.G.: new enum RETR_DEVICE
 * 26. 9.1997, H.G.: new ACTION value RETRIEVE_STAGE
 * 21.10.1997, H.G.: replace ARCHIVE_STAGE by ARCHIVE_OVER
 * 19. 1.1998, H.G.: new status flag STA_END_OF_FILE_NOT_STAGED
 *  8. 7.1998, H.G.: use MBUF_SOCK and MBUF_ADSM; set both to 16384
 *                   new filetype STREAM
 * 28. 7.1999, H.G.: new ACTION values UNSTAGE, QUERY_UNSTAGE
 * 16. 8.2000, H.G.: new ACTION values QUERY_WORKSPACE, QUERY_POOL
 *                   new structures srawPoolStatus, srawWorkSpace
 * 16. 8.2000, H.G.: mod srawPoolStatus, new srawPoolAttr
 *  5. 9.2000, H.G.: new ACTION value QUERY_STAGE
 * 22.11.2000, H.G.: enhance range of values for srawWorkSpace.iStatus
 *  1.12.2000, H.G.: new status id IDENT_NEXT_BUFFER
 * 21.12.2000, H.G.: new structure srawRequest for seek functionality
 *  1. 2.2001, H.G.: new action values ARCHIVE_STAGE, METADATA_MOD
 * 30. 5.2001, H.G.: increase MAX_TAPE_FILE from 16 to 17
 * 13. 6.2001, H.G.: new STATUS value ARCHIVE_AVAILABLE
 * 22. 6.2001, H.G.: new ACTION QUERY_RETRIEVE
 *                   replace MPORT by new def PORT_MASTER
 * 17. 7.2001, H.G.: enhance srawObjAttr by data mover info
 *                   new structure srawDataMover
 * 23. 8.2001, H.G.: enhance srawComm by stage info
 * 29.10.2001, H.G.: ported to W2000
 * 14.11.2001, H.G.: rename ACTION DELETE -> REMOVE (used in winnt.h)
 * 28. 2.2002, H.G.: new: IDENT_STAGE_xxx, IDENT_SPACE_xx
 * 15. 3.2002, H.G.: rename srawPoolAttr -> srawPoolStatusData, enhance
 * 18. 3.2002, H.G.: new define MAX_NUM_DM (<- rawservn.h)
 * 20. 3.2002, H.G.: new structure srawPoolStatusDMData
 * 27. 5.2002, H.G.: new ACTION QUERY_ARCHIVE_OVER
 *  6. 6.2002, H.G.: new identifier: IDENT_PURGE_INFO
 * 20. 8.2002, H.G.: new identifier: IDENT_SPACE_LOOK
 * 19. 9.2002, H.G.: new identifier: IDENT_CLEAN_REQUEST
 * 26. 9.2002, H.G.: new deinition PORT_RFIO_SERV
 * 29.10.2002, H.G.: new identifier: IDENT_FILELIST_REQUEST,
 *                   new define MAX_STAGE_FILE_NO
 * 22.11.2002, H.G.: new status STA_INFO, STA_PURGED, STA_NOT_PURGED
 * 13. 1.2003, H.G.: add definition PORT_MOVER
 * 30. 1.2003, H.G.: add declarations cMasterNode(i), new: MAX_MASTER
 * 13. 2.2003, H.G.: add definition MAX_MOVER
 * 24. 2.2003, H.G.: new actions SPM_REQUEST_MOVER, QUERY_ARCHIVE_STAGE
 *  8. 5.2003, H.G.: add synchronization parameters to srawDataMoverAttr
 *  9. 5.2003, H.G.: add synchronization parameters to srawComm
 * 21. 8.2003, H.G.: srawComm: new string cTapeLib,
 *                   replace ACTION values:
 *                      ARCHIVE_STAGE -> ARCHIVE_TO_STAGE
 *                      METADATA_MOD  -> ARCHIVE_FROM_STAGE
 *                   new ACTION values: QUERY_ARCHIVE_TO_STAGE,
 *                                      QUERY_ARCHIVE_FROM_STAGE
 * 28. 8.2003, H.G.: srawObjAttr: add parameter cStageUser
 * 15. 9.2003, H.G.: increase MAX_NUM_DM: 5 -> 10
 *  6.10.2003, H.G.: cNodeMaster2 now gsitsma, depc134 is backup node
 * 10.10.2003, H.G.: new identifier: IDENT_QUERY_ARCHDB
 * 15.10.2003, H.G.: new action END_SESSION
 * 31.10.2003, H.G.: remove unused definition MAX_MOVER
 *  6.11.2003, H.G.: new action QUERY_ARCHIVE_RECORD
 * 18.11.2003, H.G.: new identifier: IDENT_FULL_ARCHDISK
 * 18.12.2003, H.G.: new identifier: IDENT_ARCHIVE_FILE
 * 22. 1.2004, H.G.: add cNodeMaster2A: gsitsmb
 * 20. 2.2004, H.G.: srawObjAttr: rename iPort (unused) -> iFS
 *                   new structure srawFileSystem
 * 24. 2.2004, H.G.: new ACTION values: STAGE_FROM_CACHE, RETRIEVE_FROM_CACHE
 *                   replace ACTION values:
 *                      QUERY_ARCHIVE_TO_STAGE -> QUERY_ARCHIVE_TO_CACHE
 *                      QUERY_ARCHIVE_FROM_STAGE -> QUERY_ARCHIVE_FROM_CACHE
 *                      ARCHIVE_TO_STAGE -> ARCHIVE_TO_CACHE
 *                      ARCHIVE_FROM_STAGE -> ARCHIVE_FROM_CACHE
 *  5. 3.2004, H.G.: srawFileSystem: add cArchiveDate, cArchiveUser
 *                   new ACTION value READ_FROM_CACHE
 ****************************************************************************
 */

#define PORT_MASTER 1996        /* default port number master server */
#define PORT_MOVER  1994           /* default port number data mover */
#define PORT_RFIO_SERV 1974       /* default port number RFIO server */

#define MAX_MASTER   2               /* max number of master servers */
#define MAX_NUM_DM 10          /* max no. of data movers on platform */

#define MAX_NODE    16                       /* max length node name */
static char cNodeMaster[MAX_NODE] = "";          /* requested server */
static char cNodeMaster1[MAX_NODE] = "adsmexp";    /* server 1 (AIX) */
static char cNodeMaster2[MAX_NODE] = "gsitsma"; /* server 2 (Windows)*/
static char cNodeMaster2A[MAX_NODE] = "gsitsmb";  /* server 2 backup */
static char cNodeMaster2B[MAX_NODE] = "depc134";  /* server 2 backup */

#define MBUF_SOCK 16384               /* default buffer size sockets */
#define MBUF_ADSM 16384                  /* default buffer size ADSM */
#define MBUF_API  65536        /* default buffer size for API client */

#define MAX_FILE_SIZE 2147483647          /* max filesize 2Gbyte - 1 */
#define MAX_FILE_NO 1024         /* max number of file names in list */
#define MAX_STAGE_FILE_NO 5000 /* max no of stage file names in list */
#define MAX_FILE   128                       /* max length file name */
#define MAX_TAPE_FILE 17             /* max length file name on tape */
#define MAX_OWNER   16                    /* max length object owner */
#define MAX_OBJ_FS  32                  /* max length filespace name */
#define MAX_OBJ_HL  92          /* max length object high level name */
#define MAX_OBJ_LL  36           /* max length object low level name */
#define MAX_DATE    20           /* max length archive date and time */
#define MAX_OS       8           /* max length operating system name */
#define MAX_APPLTYPE 32               /* max length application type */
#define MAX_MC      12           /* max length Management class name */

#define HEAD_OFFSET  3       /* offset common pre-header (int array) */
#define HEAD_LEN    12           /* length common pre-header (bytes) */
#define STATUS_LEN 256            /* max length status/error message */
#define WORK_LEN    20                     /* length work space info */

/* identifiers for buffer headers transfered via sockets */
/* at least IDENT_STATUS must be <0 to avoid confusion with data */
#define IDENT_COMM         -1        /* communication control buffer */
#define IDENT_STATUS       -2        /* status buffer from/to client */
#define IDENT_QUERY        -3                 /* query result buffer */
#define IDENT_POOL         -4              /* stage pool info buffer */
#define IDENT_WORKSPACE    -5              /* work space info buffer */
#define IDENT_NEXT_BUFFER  -6            /* request next data buffer */
#define IDENT_MOVER_ATTR   -7               /* data mover attributes */
#define IDENT_STAGE_FILE   -8        /* stage file data (read cache) */
#define IDENT_SPACE_INFO   -9               /* data mover space info */
#define IDENT_PURGE_INFO  -10        /* purge request for data mover */
#define IDENT_SPACE_LOOK  -11             /* meta data DB space info */
#define IDENT_CLEAN_REQUEST  -12     /* clean request for stage pool */
#define IDENT_FILELIST_REQUEST -13  /* filelist request for stage FS */
#define IDENT_QUERY_ARCHDB -14         /* query result buffer archDB */
#define IDENT_FULL_ARCHDISK -15       /* buffer describing full disk */
#define IDENT_ARCHIVE_FILE -16    /* archive file data (write cache) */

enum ARCH_DEVICE                                   /* archive device */
{
   ARCH_ANY,                               /* any device (for query) */
   ARCH_TAPE,                                 /* standard tape class */
   ARCH_TAPE_MC,                   /* special tape class for archive */
   MGR_TAPE,                      /* tape system manager: special MC */
   ARCH_DISK,                                                /* disk */
   ARCH_TAPE_MC_2        /* alternate special tape class for archive */
};

enum RETR_DEVICE                           /* logical staging device */
{
   RETR_CLIENT,                                        /* no staging */
   RETR_STAGE_TEMP,              /* delete imediately after retrieve */
   RETR_STAGE_PERM,        /* kept, but erased if not used some time */
   RETR_STAGE_DATE              /* kept for one day, if enough space */
                       /* RETR_STAGE_DATE+n: kept n+1 days, if space */
};

enum FILETYPE                                      /* describes data */
{
   FIXED_INTS4,                /* fixed records, 4 byte int (signed) */
   STREAM                               /* record length not defined */
};

enum ACTION                                                /* action */
{
   ARCHIVE,                   /*  0: archive files */
   ARCHIVE_MGR,               /*  1: system mgr only: set owner, special MC */
   ARCHIVE_RECORD,            /*  2: archive file from client program */
   ARCHIVE_OVER,              /*  3: existing file will be overwritten
                                     client only, server: REMOVE + ARCHIVE */
   CREATE_ARCHIVE,            /*  4: system mgr only: create new archive */
   CLOSE,                     /*  5: close tape for client */
   REMOVE,                    /*  6: delete file from archive (stage pool) */
   REMOVE_MGR,                /*  7: delete file from archive (stage pool) */
   FILE_CHECK,                /*  8: only needed for client side */
   OPEN,                      /*  9: open tape for client */
   QUERY,                     /* 10: query for files */
   QUERY_ARCHIVE,             /* 11: query for files already archived */
   QUERY_ARCHIVE_MGR,         /* 12: query for files already archived */
   QUERY_REMOVE,              /* 13: query for files to be deleted */
   QUERY_REMOVE_MGR,          /* 14: query for files to be deleted */
   RETRIEVE,                  /* 15: retrieve file to client */
   RETRIEVE_RECORD,           /* 16: retrieve file to client program */
   RETRIEVE_STAGE,            /* 17: retrieve file to client via stage disk */
   STAGE,                     /* 18: retrieve file to stage disk pool */
   QUERY_UNSTAGE,             /* 19: query for files to be unstaged */
   UNSTAGE,                   /* 20: remove file from stage disk pool */
   QUERY_POOL,                /* 21: query for status of disk pools */
   QUERY_WORKSPACE,           /* 22: query for files to be staged + stage pool*/
   QUERY_STAGE,               /* 23: query for files to be staged */
   ARCHIVE_TO_CACHE,          /* 24: archive files to write cache */
   ARCHIVE_FROM_CACHE,        /* 25: archive files from write cache */
   QUERY_RETRIEVE,            /* 26: query for files to be retrieved */
   QUERY_ARCHIVE_OVER,        /* 27: query before "archive -over" action */
   QUERY_ARCHIVE_TO_CACHE,    /* 28: query before archive to write cache */
   SPM_REQUEST_MOVER,         /* 29: request data mover from SPM for action */
   QUERY_RETRIEVE_API,        /* 30: query before retrieve to client program */
   QUERY_ARCHIVE_FROM_CACHE,  /* 31: query before archive from write cache */
   END_SESSION,               /* 32: end session on server */
   QUERY_ARCHIVE_RECORD,      /* 33: query before archive from client program */
   STAGE_FROM_CACHE,          /* 34: stage files from write cache */
   RETRIEVE_FROM_CACHE,       /* 35: retr. files (via stage) from write cache */
   READ_FROM_CACHE            /* 36: read from write cache, send to other DM */
};

/* command buffer with object attributes */
typedef struct
{
   int iIdent;               /* IDENT_COMM identifies command buffer */
   int iAction;                          /* see definition of ACTION */
   int iCommLen;                         /* length of following data */
   char cNamefs[MAX_OBJ_FS];                       /* filespace name */
   char cNamehl[MAX_OBJ_HL];               /* object high level name */
   char cNamell[MAX_OBJ_LL];    /* file name / object low level name */
   char cOwner[MAX_OWNER];              /* account name of requestor */
   char cOS[MAX_OS];                        /* user operating system */
   char cApplType[MAX_APPLTYPE];                 /* application type */
   int iFileType;                      /* see definition of FILETYPE */
   int iBufsizeFile;                             /* buffer size file */
   unsigned long iFileSize;                 /* size of file in bytes */
   int iArchDev;                     /* = ARCH_DEVICE or RETR_DEVICE */
   unsigned int iObjHigh;              /* upper four bytes object Id */
   unsigned int iObjLow;               /* lower four bytes object Id */
   char cliNode[MAX_NODE];                       /* client node name */
   int iStageFSid;
     /* stage file system no. on selected data mover (0: not staged) */
   int iExecStatus;   /* execution status set by LockManager:
                         = 1: active, = 2 waiting, = 3 suppressed    */
   int iWaitTime;         /* time to wait before execution (seconds) */
   int iSynchId;                       /* id number of synch request */
   char cTapeLib[16];            /* name of ATL: "" default, "*" all */
} srawComm;

enum STATUS                                      /* status of action */
{
   STA_BEGIN_TRANS,    /*  0: server ready for data transfer */
   STA_NEXT_BUFFER,    /*  1: API client: request next sequ. buffer */
   STA_END_OF_FILE,    /*  2: */
   STA_END_OF_SESSION, /*  3: */
   STA_ERROR,          /*  4: error, end session */
   STA_ERROR_EOF,      /*  5: error, handle next file */
   STA_FILE_STAGED,    /*  6: */
   STA_END_OF_FILE_NOT_STAGED, /*  7: file retrieved, but not staged */
   STA_SEEK_BUFFER, /* 8: API client: request buffer at any position */
   ARCHIVE_AVAILABLE,  /*  9: */
   STA_SWITCH_SERVER,  /* 10: switch data mover */
   STA_INFO,           /* 11: informatory message */
   STA_PURGED,         /* 12: some/all requested files purged on DM */
   STA_NOT_PURGED      /* 13: no file could be purged on DM */
};

typedef struct                                      /* Status buffer */
{
   int iIdent;              /* IDENT_STATUS identifies status buffer */
   int iStatus;                          /* see definition of STATUS */
   int iStatusLen;                    /* length of following message */
   char cStatus[STATUS_LEN];                       /* status message */
} srawStatus;

typedef struct                      /* Request buffer for API client */
{
   int iIdent;        /* IDENT_NEXT_BUFFER identifies request buffer */
   int iStatus;                          /* see definition of STATUS */
   int iStatusLen;                       /* length of following info */
   int iSeekMode;    /* byte offset mode (see lseek C function):
                SEEK_SET: set offset to iOffset bytes
                SEEK_CUR: increase current offset for iOffset bytes
                SEEK_END: set offset to file size plus iOffset bytes */
   int iOffset;                             /* requested byte offset */
   int iBufferSize;                    /* length of requested buffer */
} srawRequest;

typedef struct                 /* Object attribute buffer */
{
   int iVersion;               /* structure version */
   char cNamefs[MAX_OBJ_FS];   /* filespace name */
   char cNamehl[MAX_OBJ_HL];   /* object high level name */
   char cNamell[MAX_OBJ_LL];   /* file name / object low level name */
   int iFileType;              /* see definition of FILETYPE */
   int iBufsizeFile;           /* buffer size file */
   unsigned long iFileSize;    /* size of file in bytes */
   char cDate[MAX_DATE];       /* archive date and time */
   char cOwner[MAX_OWNER];     /* account name of owner */
   char cOS[MAX_OS];           /* operating system archiving client */
   char cMgmtClass[MAX_MC];    /* management class */
   int iMediaClass;            /* media class */
   unsigned int iObjHigh;      /* upper four bytes object Id */
   unsigned int iObjLow;       /* lower four bytes object Id */
   unsigned int iRestoHigh;    /* upper four bytes restore order */
   unsigned int iRestoLow;     /* lower four bytes restore order */
   int iFS;                    /* filesystem number if file on disk */
   char cNode[MAX_NODE];       /* node name data mover */
   char cStageUser[MAX_OWNER]; /* account name of staging user */
} srawObjAttr;

typedef struct                 /* Query response buffer */
{
   int iIdent;                 /* IDENT_QUERY or IDENT_QUERY_ARCHDB */
   int iObjCount;              /* no. of objects matching  */
   int iAttrLen;               /* length attributes buffer of object */
   srawObjAttr objAttr;        /* attributes of object found */
                               /* appended for each object   */
} srawQueryResult;

/* stage pool attributes for one data mover */
typedef struct
{
   char cNodeName[MAX_NODE];                 /* node name data mover */
   int iMaxSizeMover; /* max size of pool on this data mover (MByte) */
   int iFreeSizeHW;         /* current free size in hardware (MByte) */
   int iFreeSize;               /* current free size in pool (MByte) */
   int iFiles;                       /* current no. of files in pool */
   int iFileSystems; /* no. of FS on this data mover:
                        = 0: all FS, > 0: list follows (srawStageFS) */
} srawPoolStatusDMData;

/* stage pool attributes */
typedef struct
{
   char cPoolName[32];                                  /* pool name */
   char cPoolOS[MAX_OS];            /* name of OS where pool resides */
   int iPoolId;                        /* pool identification number */
   int iMaxSizeHW;                     /* overall size of HW (MByte) */
   int iFreeSizeHW;               /* current free size in HW (MByte) */
   int iMaxSize;                     /* overall size of pool (MByte) */
   int iFreeSize;               /* current free size in pool (MByte) */
   int iMaxWorkSize;       /* max size of work space in pool (MByte) */
   int iFileAvail;        /* guaranteed availability of files (days) */
   int iCheckSize;
       /* threshold work space size for check of pool status (MByte) */
   int iFiles;                       /* current no. of files in pool */
   int iDataMover;                  /* number of data movers in pool */
} srawPoolStatusData;

/* for exchange with client: status buffers stage pool + WS */
typedef struct
{
   int iIdent;           /* IDENT_POOL identifies pool status header */
   int iPoolNo;                /* no. of pool + WS buffers following */
   int iStatusLen;    /* size of following data (n pools, opt. 1 WS) */
   srawPoolStatusData sPoolStatusData;      /* stage pool attributes */
} srawPoolStatus;

typedef struct                   /* server infos on requ. work space */
{
   int iIdent;/* IDENT_WORKSPACE identifies work space status header */
   int iWorkId;                  /* work space identification number */
   int iStatusLen;                       /* length of following data */
   int iWorkSizeAll;             /* size of requ. work space (MByte) */
   int iWorkFilesAll;            /* total no. of files in work space */
   int iWorkSizeSta;      /* part of requ. work space already staged */
   int iWorkFilesSta;   /* no. of files in work space already staged */
   int iWorkSizeStaTemp;   /* part of staged work space in temp pool */
   int iWorkFilesStaTemp;        /* no. of staged files in temp pool */
   int iWorkSizeEst;      /* part of requ. work space size estimated */
   int iWorkFilesEst;
                   /* no. of files in work space with size estimated */
   int iStatus; /* work space status flag:
                   = -1: problem, can't create
                   =  0: okay, space available
                   =  1: lack of space in pool
                   =  2: above allowed limit of work space
                   =  3: work space larger than pool
                   =  9: lack of space -> clean job, no sleep
                  >= 10: lack of space -> clean job, sleep iStatus min.
                 */
} srawWorkSpace;

typedef struct                           /* attributes of data mover */
{
   char cNode[MAX_NODE];                                /* node name */
   int iPort;                                            /* port no. */
   int iSocket;                                        /* socket no. */
   int iExecStatus;   /* execution status set by LockManager:
                         = 1: active, = 2 waiting, = 3 suppressed    */
   int iWaitTime;         /* time to wait before execution (seconds) */
   int iSynchId;                       /* id number of synch request */
} srawDataMoverAttr;

typedef struct    /* communication buffer with data mover attributes */
{
   int iIdent;              /* IDENT_STATUS identifies status buffer */
   int iStatus;                          /* see definition of STATUS */
   int iStatusLen;                    /* length of following message */
   srawDataMoverAttr sDataMoverAttr;     /* attributes of data mover */
} srawDataMover;

typedef struct                                   /* filesystem infos */
{
   char cOS[MAX_OS];                             /* operating system */
   char cNode[MAX_NODE];                                /* node name */
   int iFileSystem;                     /* no. of filesystem on node */
   char cArchiveDate[MAX_DATE];      /* creation date in ArchivePool */
   char cArchiveUser[MAX_OWNER];   /* account name of archiving user */
} srawFileSystem;


