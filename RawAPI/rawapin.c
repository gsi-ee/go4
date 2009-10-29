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

/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created  4.12.2000 by Horst Goeringer
 ********************************************************************
 * rawapin.c
 *   procedures for tsmcli client API (RFIO syntax)
 *   new version
 ********************************************************************
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
 *                   rfio_readdir, rfio_stat
 * 25. 9.2003, H.G.: take synchronization into account
 * 29. 4.2004, H.G.: catch action without open in advance
 ********************************************************************
 */

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#ifdef Linux
#include <sys/stat.h>
#endif

#include "rawdefn.h"
#include "rawapin.h"    /* has includes for rawcommn.c and rawclin.h */
#include "rawentn.h"

#include "rawapitd.h"
#include "rawapitd-gsi.h"
#include "rawapplcli.h"

int* piEntryList;              /* external in some utility functions */
int imySigS;         /* if = 1: CTL C specified (extern in rconnect) */
FILE *fLogFile;                /* external in some utility functions */

/* externals for ROOT: TRFIOFile.cxx */
int serrno;                               /* id system error message */
int rfio_errno;                             /* id RFIO error message */

static char rfio_errmsg[256] = "";    /* RFIO related msg for stderr */

static int iint = sizeof(int);
static int irawComm = sizeof(srawComm);
static int irawFileSystem = sizeof(srawFileSystem);
static int irawAPIFile = sizeof(srawAPIFile);
                                           /* API file control block */

static int iFileCount = 0;            /* currently open remote files */
static int iFileMax = 10;       /* API file control blocks available */
static RFILE *pAPIFile[10] =      /* ptrs to API file control blocks */
   {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/********************************************************************
 * rfio_open: open remote file in mass store managed by tsmcli
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_open(char *pcFile,                           /* file name */
              int iFlags,
              int iMode)
{
   char cModule[32] = "rfio_open";
   int iDebug = 0;
   int iRC;
   int iMaxConnect = 0;            /* try connection to servers once */

   int iMassSto = 1;   /* =1: contact GSI mass storage (default)
                          =0: contact RFIO read/write server         */
   int iStage = 0;                        /* = 1: file in stage pool */
   int iCache = 0;                       /* = 1: file in write cache */
   int iStatus = 0;
   int iStatusLen = 0;
   int iIdent;
   int iBufl;
   int iDataMoverSelect = 0;

   int iAction = 0;                           /* =1: read, =2: write */
   int iError = 0;
   int iSendEOS = 0;
     /* =1: send EOS to connected servers (master and/or data mover) */

   int iFileId;             /* file identifier for server connection */
   char cServer[24] = "";
   char cNodeMaster[MAX_NODE] = "adsmexp";
   int iSockMaster = -1;          /* socket for connection to master */
   int iSockMaster1 = -1;      /* for connection to alternate master */
   char cNodeMover[MAX_NODE] = "";
   int iSockMover = -1;            /* socket for connection to mover */

   int iSeekMode = -1;                           /* default: no seek */
   int iOffset = 0;                              /* default: no seek */
   int iBufsizeAct = 0;
   int iBufnoServ = 0;   /* no. of buffers transfered from/to server */
   int iDataServ = 0;      /* no. of bytes transfered from/to server */

   int ii;
   char *pcc, *pcc1;
   char cTemp[256] = "";
   char cMsg[128] = "                                                                                                                              ";

   char cNamefs[MAX_OBJ_FS] = "", *pcNamefs;      /* filespace name */
   char cNamehl[MAX_OBJ_HL] = "", *pcNamehl;     /* high level name */
   char cNamell[MAX_OBJ_LL] = "", *pcNamell;     /* low  level name */

   char cOwner[DSM_MAX_OWNER_LENGTH] = "";
   char cliNode[MAX_NODE] = "";

   srawDataMover sDataMoverSelect;
                      /* buffer with data mover attrs sent by server */
   srawDataMoverAttr *pDataMoverSelect;       /* selected data mover */

   srawStatus sStatus;
   srawAPIFile *pcurAPIFile;
   srawComm *pCommAPI;  /* communication buffer in API control block */
   srawComm *pCommServ;            /* copy for servers in net format */
   srawObjAttr *pQueryAPI;                      /* query information */

   srawFileSystem sFileSystemServ;
       /* if file in ArchivePool: FS infos for servers in net format */

   char *pBufQuery;
   srawQueryResult sQuery;
   srawObjAttr *pQAttr;

   fLogFile = stdout;

   pcNamefs = cNamefs;
   pcNamehl = cNamehl;
   pcNamell = cNamell;

   if (iFlags == O_RDONLY)
      iAction = 1;
   else
      iAction = 2;

   if (iDebug)
   {
      printf("\n-D- begin %s", cModule);
      if (iAction == 1) printf(
         " for read\n    %d remote files currently open\n",
         iFileCount);
      else printf(
         " for write\n    %d remote files currently open\n",
         iFileCount);
   }

   ii = 0;
   if (iFileCount > 0) while (ii < iFileMax)
   {
      if (pAPIFile[ii] == NULL)
         break;
      ii++;
   }

   if (ii == iFileMax)
   {
      printf("-E- max %d remote files may be open simultaneously\n",
             iFileMax);
      return -1;
   }

   pAPIFile[ii] = (srawAPIFile *) malloc(irawAPIFile);
   if (pAPIFile[ii] == NULL)
   {
      printf("-E- allocating file control block no. %d (%d byte)\n",
             ii, irawAPIFile);
      perror("    ");
      return -1;
   }

   iFileId = ii;
   iFileCount++;
   if (iDebug) printf(
      "    file control block no. %d allocated (%d byte)\n",
      ii, irawAPIFile);

   pcurAPIFile = pAPIFile[ii];
   pcurAPIFile->iFileId = iFileId;
   pcurAPIFile->iFlags = iFlags;
   pcurAPIFile->iMode = iMode;
   pcurAPIFile->iSockMaster = -1;
   pcurAPIFile->iSockMover = -1;
   pcurAPIFile->iSeekMode = iSeekMode;
   pcurAPIFile->iOffset = iOffset;
   pcurAPIFile->iBufsizeAct = iBufsizeAct;
   pcurAPIFile->iBufnoServ = iBufnoServ;
   pcurAPIFile->iDataServ = iDataServ;

   pCommAPI = &(pcurAPIFile->sCommAPI);
   pCommAPI->iIdent = IDENT_COMM;
   pCommAPI->iCommLen = irawComm - HEAD_LEN;

   pQueryAPI = &(pcurAPIFile->sQueryAPI);
   pCommServ = &(pcurAPIFile->sCommServ);

   /**************** analyze remote file specification **************/

   if ( (strchr(pcFile, *pcStar) != NULL) ||
        (strchr(pcFile, *pcQM) != NULL) ||
        (strchr(pcFile, *pcPerc) != NULL) )
   {
      printf("-E- generic TSM representation '%s' not allowed\n",
             pcFile);
      return -1;
   }

   /* handle prefixes to object name */
   pcc = (char *) strchr(pcFile, *pcDevDelim);
   if (pcc != NULL)
   {
      pcc++;                                /* skip device delimiter */
      /* check for "rfio:" */
      strncpy(cTemp, pcFile, 8);
      pcc1 = cTemp;
      while (*pcc1 != '\0')
      { *pcc1++ = tolower(*pcc1); }

      iRC = strncmp(cTemp, "rfio", 4);
      if (iRC == 0)
          /* rfiocopy:node:object or rfio:node:object or rfio:object */
      {
         iRC = strcmp(cTemp, "rfiocopy");
         if (iRC == 0)
         {
            iMassSto = 0;
            strcpy(cServer, "RFIO server");
            iPortMaster = PORT_RFIO_SERV;
            iPortMover = PORT_RFIO_SERV;
         }

         pcc1 = (char *) strchr(pcc, *pcDevDelim);
         if (pcc1 != NULL)                       /* pcc: node:object */
         {
            strcpy(cTemp, pcc);             /* starts with node name */
            pcc1++;                         /* skip device delimiter */
            if (iMassSto == 0)
               strcpy(pcNamehl, pcc1);     /* fs name has no meaning */
            else
               strcpy(pcNamefs, pcc1);
         }
         else                                         /* pcc: object */
         {
            if (iMassSto == 0)
            {
               printf("-E- node name RFIO server missing\n");
               return -1;
            }
            strcpy(cTemp, "");
            strcpy(pcNamefs, pcc);
         }
      }
      else                                       /* type node:object */
      {
         if (iDebug)
         {
            iPortMaster -= 10;                        /* test system */
            iPortMover -= 10;
         }
         strcpy(cTemp, pcFile);             /* starts with node name */
         strcpy(pcNamefs, pcc);
      }

      pcc = (char *) strchr(cTemp, *pcDevDelim);
      if (pcc != NULL)
      {
         strncpy(pcc, "\0", 1);              /* terminates node name */
         if (strlen(cTemp) <= 1)
         {
            if (iMassSto == 0)
            {
               printf("-E- node name RFIO server missing\n");
               return -1;
            }
         }
         else
            strcpy(cNodeMaster, cTemp);
      }
   }
   else /* no prefix (node name and/or rfio */
      strcpy(pcNamefs, pcFile);

   if (iMassSto == 0)
      strcpy(cTemp, pcNamehl);
   else
   {
      strcpy(cTemp, pcNamefs);
      strcpy(cServer, "mass storage");
   }

   pcc = cTemp;
   while (*pcc == *pcDevDelim)
   {
      pcc++;
   }
   strcpy(cTemp, pcc);

   if (iDebug) printf("    %s node '%s', TSM object '%s'\n",
      cServer, cNodeMaster, cTemp);

   /* now handle object name */
   pcc = (char *) strchr(cTemp, *pcObjDelim);
   if (iMassSto == 1)
   {
      if (pcc == NULL)
      {
         printf("-E- invalid remote file name '%s'\n", pcFile);
         return -1;
      }

      if (strncmp(pcc, cTemp, 1) == 0) /* skip leading obj delimiter */
         pcc++;
      pcc = (char *) strchr(pcc, *pcObjDelim);
      if (pcc == NULL)
      {
         printf("-E- invalid remote file name '%s'\n", pcFile);
         return -1;
      }

      strcpy(pcNamehl, pcc);   /* starts with high level object name */
      strncpy(pcc, "\0", 1);                   /* terminates FS name */
      strcpy(pcNamefs, cTemp);

      if (strlen(cNamefs) <= 1)
      {
         printf("-E- invalid archive '%s' in remote file name '%s'\n",
                pcNamefs,  pcFile);
         return -1;
      }

      strcpy(pCommAPI->cNamefs, rawGetFSName(pcNamefs));
      pcc = pcNamehl;
      pcc++;                              /* skip leading delimiter */
      pcc = (char *) strrchr(pcc, *pcObjDelim);
      if (pcc == NULL)
      {
         printf("-E- invalid remote file name '%s'\n", pcFile);
         return -1;
      }
      strcpy(pcNamell, pcc);   /* starts with low level object name */
      strncpy(pcc, "\0", 1);   /* terminates high level object name */

      strcpy(pCommAPI->cNamehl, rawGetHLName(pcNamehl));
      iRC = rawGetLLName(pcNamell, pcObjDelim, pCommAPI->cNamell);
      if (iRC)
      {
         printf("-E- checking low level object name: %s\n",
                pCommAPI->cNamell);
         return -1;
      }
   } /* (iMassSto == 1) */
   else
   {
      strcpy(pcNamefs, "");                /* fs name has no meaning */
      strcpy(pCommAPI->cNamefs, "");

      if (pcc == NULL)                 /* rel file name without path */
      {
         strcpy(pcNamehl, "");             /* fs name has no meaning */
         strcpy(pCommAPI->cNamehl, "");
         strcpy(pcNamell, cTemp);
      }
      else                               /* full file name with path */
      {
         strcpy(pcNamehl, cTemp);
         pcc = (char *) strrchr(pcNamehl, *pcObjDelim);
         strcpy(pcNamell, pcc);                /* low level obj name */
         strncpy(pcc, "\0", 1);     /* terminate high level obj name */
         strcpy(pCommAPI->cNamehl, pcNamehl);
      }
      strcpy(pCommAPI->cNamell, pcNamell);

   } /* (iMassSto == 0) */

   strcpy(pCommAPI->cTapeLib, "0");          /* default tape library */

   if ( iAction == 1)
      pCommAPI->iAction = QUERY_RETRIEVE_API;      /* at first query */
   else
   {
      if (iMassSto)
         pCommAPI->iAction = QUERY_ARCHIVE_RECORD; /* at first query */
      else
         pCommAPI->iAction = ARCHIVE;       /* RFIO server: no query */
   }

   if (iDebug) printf(
      "    FS name '%s', HL name '%s', LL name '%s', action %d\n",
      pCommAPI->cNamefs, pCommAPI->cNamehl, pCommAPI->cNamell,
      pCommAPI->iAction);

   /******************** get client information *********************/

   strcpy(cOwner, rawGetUserid());
   iRC = gethostname(cliNode, MAX_NODE);
   if (iRC)
   {
      printf("-E- getting client host name: %s\n", strerror(iRC));
      return -1;
   }

   if (iDebug)
      printf("    user %s on node %s, platform %s\n",
             cOwner, cliNode, cOS);            /* cOS from rawcli.h */

   strcpy(pCommAPI->cApplType, cApplType);
   strcpy(pCommAPI->cOwner,  cOwner);
   strcpy(pCommAPI->cliNode, cliNode);
   strcpy(pCommAPI->cOS, cOS);
   pCommAPI->iSynchId = 0;

   pCommServ->iIdent = htonl(pCommAPI->iIdent);
   pCommServ->iAction = htonl(pCommAPI->iAction);
   pCommServ->iCommLen = htonl(pCommAPI->iCommLen);
   pCommServ->iBufsizeFile = htonl(0);
   pCommServ->iSynchId = htonl(0);
   strcpy(pCommServ->cNamefs, pCommAPI->cNamefs);
   strcpy(pCommServ->cNamehl, pCommAPI->cNamehl);
   strcpy(pCommServ->cNamell, pCommAPI->cNamell);
   strcpy(pCommServ->cOwner,  pCommAPI->cOwner);
   strcpy(pCommServ->cOS, pCommAPI->cOS);
   strcpy(pCommServ->cApplType, pCommAPI->cApplType);
   strcpy(pCommServ->cliNode, pCommAPI->cliNode);
   strcpy(pCommServ->cTapeLib, pCommAPI->cTapeLib);

   /***************** connect to alternate master ********************/

   if ( (iMassSto == 1) && (iAction == 2) )
   {
      strcpy(cServer, "alternate server");

      if (iDebug) printf(
         "-D- trying to connect to %s %s:%d\n",
         cServer, cNodeMaster1, iPortMaster);

      iRC = rconnect(cNodeMaster1, iPortMaster,
                     &iMaxConnect, &iSockMaster1);
      if (iRC)
      {
         printf("-E- cannot connect to %s %s:%d\n",
                cServer, cNodeMaster1, iPortMaster);
         printf(
            "-W- check if file %s in alternate ATL already exists not possible\n",
            pcFile);

         goto gRequServer;
      }

      if (iDebug)
      {
         printf("    successfully connected to %s %s:%d",
                cServer, cNodeMaster1, iPortMaster);
         if (iMaxConnect) printf(" after %ds", iMaxConnect);
         printf(" (socket %d)\n", iSockMaster1);
      }

      /********************* perform query **************************/

      if (iDebug) printf(
         "    query file %s in alternate mass storage\n", pcFile);

      pBufQuery = (char *) &sQuery;
      pQAttr = &(sQuery.objAttr);
      iRC = rawQueryFile(iSockMaster1, pCommServ, (void *) &pBufQuery);
      if (iRC < 0)
      {
         printf(
            "-W- file %s: query in alternate mass storage failed, rc = %d\n",
            pcFile, iRC);
         goto gRequServer;
      }

      if (iRC == 0)
      {
         if (iDebug) printf(
            "    file %s not yet available in alternate mass storage\n",
            pcFile);
      }
      else
      {
         sprintf(rfio_errmsg,
            "-E- file %s already available in alternate mass storage\n",
            pcFile);
         fprintf(stdout, "%s", rfio_errmsg);

         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }
   } /* (iMassSto == 1) && (iAction == 2) */

   /********************** connect to server *************************/

gRequServer:
   if (iMassSto)
   {
      if (iSockMaster1 >= 0)
      {
         iStatus = STA_END_OF_SESSION;
         iRC = rawSendStatus(iSockMaster1, iStatus, NULL);
         if (iRC <= 0) printf(
            "-E- sending status buffer (EOS) to alternate server %s\n",
            cNodeMaster1);
         else if (iDebug) printf(
           "    status buffer (EOS) sent to master server %s (%d bytes)\n",
           cNodeMaster1, iRC);

         if (iDebug)
            printf("    close connection to master %s:%d, socket %d\n",
                   cNodeMaster1, iPortMaster, iSockMaster1);
         iRC = shutdown(iSockMaster1, 2);
         if (iDebug)
            printf("    rc shutdown = %d\n", iRC);
         iRC = close(iSockMaster1);
         if (iDebug)
            printf("    rc close = %d\n", iRC);

         iSockMaster1 = -1;

      } /* (iSockMaster1 >= 0) */

      strcpy(cServer, "master");
   }

   if (iDebug) printf(
      "-D- trying to connect to %s %s:%d\n",
      cServer, cNodeMaster, iPortMaster);

   iRC = rconnect(cNodeMaster, iPortMaster, &iMaxConnect, &iSockMaster);
   if (iRC)
   {
      printf("-E- cannot connect to %s %s:%d\n",
             cServer, cNodeMaster, iPortMaster);
      iSendEOS = 0;
      iError = 1;
      goto gClose;
   }

   strcpy(pcurAPIFile->cNodeMaster, cNodeMaster);
   pcurAPIFile->iSockMaster = iSockMaster;

   if (iDebug)
   {
      printf("    successfully connected to %s %s:%d",
             cServer, cNodeMaster, iPortMaster);
      if (iMaxConnect) printf(" after %ds", iMaxConnect);
      printf(" (socket %d)\n", iSockMaster);
   }

   if (iMassSto == 0)
   {
      if (iAction == 1)
      {
         sprintf(rfio_errmsg,
            "-E- RFIO read not supported for RFIO write server\n");
         fprintf(stdout, "%s", rfio_errmsg);

         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }

      /* write: master and mover identical */
      strcpy(cNodeMover, cNodeMaster);
      strcpy(pcurAPIFile->cNodeMover, cNodeMaster);
      iSockMover = iSockMaster;
      pcurAPIFile->iSockMover = iSockMaster;

      iSockMaster = -1;
      pcurAPIFile->iSockMaster = -1;

   } /* (iMassSto == 0) */
   else
   {
      /********************* perform query **************************/

      if (iDebug)
         printf( "    query file %s in mass storage\n", pcFile);

      pBufQuery = (char *) &sQuery;
      pQAttr = &(sQuery.objAttr);
      iRC = rawQueryFile(iSockMaster, pCommServ, (void *) &pBufQuery);
      if (iRC < 0)
      {
         sprintf(rfio_errmsg,
                "-E- file %s: query failed, rc = %d\n", pcFile, iRC);
         fprintf(stdout, "%s", rfio_errmsg);

         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }

      if (iRC == 0)
      {
         if (iAction == 1)                                   /* read */
         {
            sprintf(rfio_errmsg,
               "-E- file %s not available in mass storage\n", pcFile);
            fprintf(stdout, "%s", rfio_errmsg);

            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }

         if (iDebug) printf(
            "    file %s not yet available in mass store\n", pcFile);
      }
      else
      {
         if (iAction == 2)                                  /* write */
         {
            sprintf(rfio_errmsg,
               "-E- file %s already available in mass storage\n",
               pcFile);
            fprintf(stdout, "%s", rfio_errmsg);

            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }

         memcpy(pQueryAPI, pQAttr, sizeof(srawObjAttr));
         if (iDebug)
         {
            printf("    file %s available in mass store:\n", pcFile);

            if (iRC > 1)
               printf("-W- %d versions of file %s exist\n",
                      iRC, pcFile);
            rawQueryString(pQueryAPI, 0, cMsg);
            printf("    %s", cMsg);
         }

         if ( (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_STAGE) ||
              (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_INCOMPLETE) ||
              (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_LOCKED) )
         {
            strcpy(cNodeMover, pQAttr->cNode);
            iStage = ntohl(pQAttr->iFS);
            if (iDebug)
            {
               printf("    file %s staged in FS %d of DM %s",
                   pcFile, iStage, cNodeMover);
               if (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_INCOMPLETE)
                  printf(", still incomplete)\n");
               else if (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_LOCKED)
                  printf(", already locked)\n");
               else printf("\n");
            }
         }
         else if (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_CACHE)
         {
            iCache = 1;

            /* specify location of cache data */
            strcpy(sFileSystemServ.cOS, "Windows");  /* DDD */
            strcpy(sFileSystemServ.cNode, pQAttr->cNode);
            sFileSystemServ.iFileSystem = pQAttr->iFS; /* net format */
            strcpy(sFileSystemServ.cArchiveDate, pQAttr->cDate);
                                                     /* archive date */
            strcpy(sFileSystemServ.cArchiveUser, pQAttr->cOwner);
                                                /* user who archived */

            strcpy(cNodeMover, pQAttr->cNode);
            iStage = ntohl(pQAttr->iFS);
            if (iDebug) printf(
               "    file %s cached in FS %d of DM %s\n",
               pcFile, iStage, cNodeMover);
         }
         else
         {
            iStage = 0;
            if (iDebug)
               printf("    file %s not yet staged\n", pcFile);
         }
      } /* iRC(rawQueryFile) > 0 */

      /********* get name of data mover selected by master ***********/

      if ( ( (iAction == 1) &&
             ((iStage == 0) || (iCache == 1)) ) ||
           (iAction == 2) )
      {
         if (iDebug) printf(
            "-D- receiving attributes of data mover selected by master\n");

         pcc = (char *) &sDataMoverSelect;
         iIdent = IDENT_MOVER_ATTR;                      /* expected */
         iStatus = STA_SWITCH_SERVER;                    /* expected */
         iRC = rawRecvHeadC(iSockMaster, pcc,
                            iIdent, iStatus, cMsg);
         if (iRC != HEAD_LEN)
         {
            if (iRC < 0)
               printf("-E- %s: receiving data mover attr header\n%s",
                      cModule, cMsg);
            else
            {
               printf("-E- %s: received error status from master server",
                      cModule);
               if (iRC)
                  printf("\n%s" , cMsg);
               else
                  printf("\n");
            }

            iSendEOS = 1;
            iError = 1;
            goto gClose;
         }
         if (iDebug) printf(
            "    header of data mover attr received (%d bytes)\n", iRC);

         iStatusLen = ntohl(sDataMoverSelect.iStatusLen);

         /* get data part of data mover info */
         pcc = (char *) &(sDataMoverSelect.sDataMoverAttr);
         iBufl = sizeof(srawDataMover) - HEAD_LEN;
         ii = iBufl;
         while(iBufl > 0)
         {
            if ( (iRC = recv( iSockMaster, pcc, iBufl, 0 )) < 0 )
            {
               printf("-E- %s: receiving data part of data mover info\n",
                      cModule);
               perror("-E- receiving data part of data mover info");

               iSendEOS = 1;                       /* may also fail! */
               iError = 1;
               goto gClose;
            }
            iBufl -= iRC;
            pcc += iRC;
         }

         pDataMoverSelect = &(sDataMoverSelect.sDataMoverAttr);
         iDataMoverSelect = ii / sizeof(srawDataMoverAttr);
                             /* no. of data movers sent: should be 1 */
         strcpy(cNodeMover, pDataMoverSelect->cNode);

         if (iDebug)
         {
            printf(
               "    data part of data mover info received (%d byte)\n",
               ii);
            printf("    %d data mover selected: %s",
                   iDataMoverSelect, cNodeMover);
            if (ntohl(pDataMoverSelect->iSynchId)) printf(
               ", synchId %d, status %d, wait for %d seconds\n",
               ntohl(pDataMoverSelect->iSynchId),
               ntohl(pDataMoverSelect->iExecStatus),
               ntohl(pDataMoverSelect->iWaitTime));
            else
               printf("\n");

         }
         else printf(
            "-I- staging file to data mover %s ...\n", cNodeMover);

         if (iDataMoverSelect != 1)
         {
            printf(
               "-W- expected 1 selected data mover, ignore remaining %d\n",
               --iDataMoverSelect);
             iDataMoverSelect = 1;
         }
      } /* ( ( (iAction == 1) && ((iStage == 0) || (iCache == 1)) ) ||
             (iAction == 2) */

      /* update communication buffer with query infos */
      if (iAction == 1)
      {
         pCommServ->iFileType = pQAttr->iFileType;
         pCommServ->iBufsizeFile = pQAttr->iBufsizeFile;
         pCommServ->iFileSize = pQAttr->iFileSize;
         pCommServ->iObjHigh = pQAttr->iObjHigh;
         pCommServ->iObjLow = pQAttr->iObjLow;
         pCommServ->iStageFSid = htonl(iStage);
      }
      else
      {
         pCommServ->iFileType = htonl(STREAM);
         pCommServ->iBufsizeFile = htonl(0);
         pCommServ->iFileSize = htonl(0);
         pCommServ->iObjHigh = htonl(0);
         pCommServ->iObjLow = htonl(0);
         pCommServ->iStageFSid = htonl(0);
      }

      /* provide query infos to other API procedures */
      pCommAPI->iFileType = ntohl(pCommServ->iFileType);
      pCommAPI->iBufsizeFile = ntohl(pCommServ->iBufsizeFile);
      pCommAPI->iFileSize = ntohl(pCommServ->iFileSize);
      pCommAPI->iObjHigh = ntohl(pCommServ->iObjHigh);
      pCommAPI->iObjLow = ntohl(pCommServ->iObjLow);
      pCommAPI->iStageFSid = ntohl(pCommServ->iStageFSid);

      if (iStage)
      {
         pCommServ->iExecStatus = htonl(0);
         pCommServ->iSynchId = htonl(0);

         pCommAPI->iExecStatus = 0;
         pCommAPI->iSynchId = 0;
      }
      else
      {
         pCommServ->iExecStatus = pDataMoverSelect->iExecStatus;
         pCommServ->iWaitTime = pDataMoverSelect->iWaitTime;
         pCommServ->iSynchId = pDataMoverSelect->iSynchId;

         pCommAPI->iExecStatus = ntohl(pDataMoverSelect->iExecStatus);
         pCommAPI->iWaitTime = ntohl(pDataMoverSelect->iWaitTime);
         pCommAPI->iSynchId = ntohl(pDataMoverSelect->iSynchId);
      }

      /********************* connect to data mover *******************/

      if (iDebug) printf(
         "-D- trying to connect to data mover %s:%d\n",
         cNodeMover, iPortMover);

      iRC = rconnect(cNodeMover, iPortMover, &iMaxConnect, &iSockMover);
      if (iRC)
      {
         printf("-E- cannot connect to data mover %s:%d\n",
                cNodeMover, iPortMover);
         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }

      if (iDebug)
      {
         printf("    successfully connected to data mover %s:%d",
                cNodeMover, iPortMover);
         if (iMaxConnect)
            printf(" after %ds", iMaxConnect);
         printf(" (socket %d)\n", iSockMover);
      }

      strcpy(pcurAPIFile->cNodeMover, cNodeMover);
      pcurAPIFile->iSockMover = iSockMover;
      strcpy(cServer, "data mover");

      /******************** close connection to master ***************/

      /* because read from data mover may take a long time! */
      if (iSockMaster)
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

      } /* (iSockMaster) */
   } /* (iMassSto) */

   if (iAction == 1)
   {
      if (iCache == 1)
      {
         /* request file copy from write cache to stage pool */
         pCommAPI->iAction = RETRIEVE_RECORD;
         pCommAPI->iCommLen += irawFileSystem;

         pCommServ->iAction = htonl(RETRIEVE_RECORD);
         pCommServ->iCommLen = htonl(pCommAPI->iCommLen);
      }
      else
      {
         /* prepare reading of records from data mover */
         pCommAPI->iAction = RETRIEVE_RECORD;
         pCommServ->iAction = htonl(RETRIEVE_RECORD);
      }
   }
   else
   {
      /* prepare writing of records to server/data mover */
      pCommAPI->iAction = ARCHIVE_RECORD;
      pCommAPI->iArchDev = ARCH_DISK;

      pCommServ->iAction = htonl(pCommAPI->iAction);
      pCommServ->iArchDev = htonl(pCommAPI->iArchDev);
   }

   if (iDebug)
   {
      printf(
         "-D- send command buffer to %s: id: %d, action %d, len %d\n",
         cServer, pCommAPI->iIdent,
         pCommAPI->iAction,
         pCommAPI->iCommLen);
      if ( (iMassSto) && (iAction == 1) )
         printf("    size %d bytes, record size %d, obj id %d-%d\n",
                pCommAPI->iFileSize,
                pCommAPI->iBufsizeFile,
                pCommAPI->iObjHigh,
                pCommAPI->iObjLow);
   }

   pcc = (char *) pCommServ;
   if ( (iRC = send(iSockMover, pcc, irawComm, 0 )) < 0 )
   {
      printf("-E- sending command buffer to %s (rc = %d)\n",
             cServer, iRC);
      perror("    ");

      iSendEOS = 1;
      iError = 1;
      goto gClose;
   }
   if (irawComm != iRC)
   {
      printf(
         "-E- incomplete command buffer sent to %s (%d of %d bytes)\n",
         cServer, iRC, irawComm);

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
      iRC = send(iSockMover, pcc, irawFileSystem, 0);
      if (irawFileSystem != iRC)
      {
         if (iRC < 0)
         {
            printf(
              "-E- sending filesystem buffer to data mover %s failed\n",
              cNodeMover);
            perror("    ");
         }
         else printf(
            "-E- incomplete filesystem buffer sent to data mover %s\n",
            cNodeMover);

         iSendEOS = 1;
         iError = 1;
         goto gClose;
      }

     if (iDebug) printf(
        "    write cache info (%d bytes) sent to data mover %s\n",
        irawFileSystem, cNodeMover);

   } /* (iCache == 1) */

   /* look for reply from server/data mover */
   memset(&sStatus, 0X00, sizeof(srawStatus));
   pcc = (char *) &sStatus;
   iRC = rawRecvStatus(iSockMover, &pcc);
   if (iRC < HEAD_LEN)
   {
      printf("-E- receiving status buffer from %s", cServer);
      if (iRC > 0)
         printf(": (%d byte)\n", iRC);
      else
         printf("\n");
      perror("    ");

      iSendEOS = 1;
      iError = 1;
      goto gClose;
   }

   iStatus = ntohl(sStatus.iStatus);
   if (iRC > HEAD_LEN)
   {
      printf("\n-E- message received from %s:\n", cServer);
      printf("    %s\n", sStatus.cStatus);

      iSendEOS = 1;
      iError = 1;
      goto gClose;
   }

   iError = 0;
   if (iDebug)
      printf("    status (%d) received from %s (%d bytes)\n",
             iStatus, cServer, iRC);

gClose:
   if (iSockMaster >= 0)
   {
      /* stop server process */
      if (iSendEOS)
      {
         iStatus = STA_END_OF_SESSION;
         iRC = rawSendStatus(iSockMaster, iStatus, NULL);
         if (iRC <= 0)
            printf("-E- %s: sending status buffer to master (EOS)\n",
                   cModule);
         else if (iDebug) printf(
            "    status buffer (EOS) sent to master (%d bytes)\n", iRC);
      }

      if (iDebug)
         printf("    close connection %d to master\n", iSockMaster);
      iRC = shutdown(iSockMaster, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSockMaster);
      if (iDebug)
         printf("    rc close = %d\n", iRC);
   }

   if (iSockMaster1 >= 0)
   {
      iRC = rawSendStatus(iSockMaster1, iStatus, NULL);
      if (iRC <= 0) printf(
         "-E- sending status buffer (EOS) to alternate server %s\n",
         cNodeMaster1);
      else if (iDebug) printf(
        "-D- status buffer (EOS) sent to master server %s (%d bytes)\n",
        cNodeMaster1, iRC);

      if (iDebug)
         printf("    close connection to master %s:%d, socket %d\n",
                cNodeMaster1, iPortMaster, iSockMaster1);
      iRC = shutdown(iSockMaster1, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSockMaster1);
      if (iDebug)
         printf("    rc close = %d\n", iRC);

   } /* (iSockMaster1 >= 0) */

   if ( (iError) && (iSockMover >= 0) )
   {
      if (iSendEOS)
      {
         /* stop server/data mover process */
         iStatus = STA_END_OF_SESSION;
         iRC = rawSendStatus(iSockMover, iStatus, NULL);
         if (iRC <= 0)
            printf("-E- %s: sending status buffer (EOS) to %s\n",
                   cModule, cServer);
         else if (iDebug) printf(
            "    status buffer (EOS) sent to %s (%d bytes)\n",
            cServer, iRC);
      }

      if (iDebug)
         printf("    close connection %d to %s\n", iSockMover, cServer);
      iRC = shutdown(iSockMover, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSockMover);
      if (iDebug)
         printf("    rc close = %d\n", iRC);
   }

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   if (iError)
      return -1;
   else
      return iFileId;

} /* rfio_open */

/********************************************************************
 * rfio_close: close remote file in mass store managed by tsmcli
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
   char cServer[16] = "server";
   srawAPIFile *pcurAPIFile;

   if (iDebug) printf("\n-D- begin %s: close remote file %d\n",
      cModule, iFileId);

   if (iFileCount == 0)
   {
      printf("-E- no remote file available for close\n");
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
      printf("-E- remote file %d cannot be closed: not found\n",
             iFileId);
      return -1;
   }

   pcurAPIFile = pAPIFile[iFileId];

   iSockMover = pcurAPIFile->iSockMover;
   if (iSockMover)
   {
      /* stop server/data mover process */
      iStatus = STA_END_OF_SESSION;
      iRC = rawSendStatus(iSockMover, iStatus, NULL);
      if (iRC <= 0)
         printf("-E- %s: sending status buffer (EOS) to %s\n",
                cModule, cServer);
      else if (iDebug) printf(
        "    status buffer (EOS) sent to %s (%d bytes)\n",
        cServer, iRC);

      if (iDebug)
         printf("    close connection %d to %s\n", iSockMover, cServer);
      iRC = shutdown(iSockMover, 2);
      if (iDebug)
         printf("    rc shutdown = %d\n", iRC);
      iRC = close(iSockMover);
      if (iDebug)
         printf("    rc close = %d\n", iRC);
   }

   free( pAPIFile[iFileId] );
   pAPIFile[iFileId] = NULL;
   iFileCount--;

   if (iDebug)
   {
      printf(
         "    remote file control block deleted (%d still active)\n",
         iFileCount);
      printf("-D- end %s\n\n", cModule);
   }

   return 0;

} /* rfio_close */

/********************************************************************
 * rfio_read: read remote file in mass store managed by tsmcli
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_read(int iFileId,                               /* file id */
              char *pcBuffer,                        /* data buffer */
              int iItems)                   /* number of data bytes */
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

   char cMsg[128] = "";
   char *pcc;
   int ii = 0;
   int iBuf;
   int *pint;

   srawAPIFile *pcurAPIFile;
   srawComm *pCommAPI;  /* communication buffer in API control block */
   srawComm *pCommServ;            /* copy for servers in net format */

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (iFileCount == 0)
   {
      printf("-E- no remote file open for read\n");
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
      printf("-E- remote file %d cannot be read: not opened\n",
             iFileId);
      return -1;
   }

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
   iRC = rawSendRequest(iSockMover, iSeekMode, iOffset, iItems);
   if (iRC != 0)
   {
      printf("-E- sending request for next data buffer\n");
      return -1;
   }

   /* reset seek options to default (no seek) */
   if (iSeekMode != -1)
   {
      pcurAPIFile->iSeekMode = -1;
      pcurAPIFile->iOffset = 0;
   }

   pcurAPIFile->iBufsizeAct = iItems;          /* keep current value */

   /********************** retrieve next buffer **********************/

   /* get size retrieve buffer */
   pcc = (char *) &iBufsizeRecv;
   iBuf = iint;
   while(iBuf > 0)
   {
gRetryLen:
      if ( (iRC = recv( iSockMover, pcc, iBuf, 0 )) < 0 )
      {
         if (iDebug)
         {
            printf("-E- receiving data length from server (buffer %d)",
                   iBufnoServ);
            perror("EEE ");
         }
         goto gRetryLen;
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
   if (iDebug == 2) printf("%d_", iBufsizeRecv);

   if (iBufsizeRecv < 0)
   {
      if (iBufsizeRecv != IDENT_STATUS)
      {
         printf("-E- invalid status header received (%d)\n",
                iBufsizeRecv);
         return -1;
      }
      if (iDebug) printf("-D- status header found\n");

      /* receive remainder of status header */
      pcc = pcBuffer;
      iBuf = HEAD_LEN - 4;                   /* iIdent already read */
      while(iBuf > 0)             /* get remainder of status header */
      {
         if ( (iRC = recv( iSockMover, pcc, iBuf, 0 )) < 0 )
         {
           printf(
              "-E- receiving status header from data mover (buffer %d)",
              iBufnoServ);
           perror("    ");
           return -1;
         }
         if (iDebug == 2) printf("%d.", iRC);

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
         printf("-E- unexpected status from data mover: %d\n", iStatus);
         return -1;
      }

      if ( (iStatus == STA_END_OF_FILE) ||
           (iStatus == STA_END_OF_SESSION) )
      {
         if (iDebug) printf("    EOF on input\n");

         if (iDataServ == 0)                          /* empty file */
         {
            printf("\n-E- %s%s%s empty\n",
                   pCommServ->cNamefs,
                   pCommServ->cNamehl,
                   pCommServ->cNamell);
            return -1;
         }
         else return 0;
      }
      else
      {
         pint++;                           /* points to status size */
         iStatusLen = ntohl(pint);
         if (iStatusLen > 0)
         {
            iRC = rawRecvError(iSockMover, iStatusLen, &pcc);
            if (iRC < 0)
            {
              printf(
                 "-E- receiving status message from data mover, rc = %d\n",
                 iRC);
              return -1;
            }

            if ( (iStatus == STA_ERROR) ||
                 (iStatus == STA_ERROR_EOF) )
            {
               printf("-E- received error status from data mover:\n");
               printf("%s\n", pcc);
               return -1;
            }

            /* warning from data mover */
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
      iItems = iBufsizeRecv;
   }

   /* recv loop over buffers of size iBufsizeRecv */
   iBuf = iBufsizeRecv;
   pcc = pcBuffer;
   while(iBuf > 0)
   {
gRetryData:
      if ( (iRC = recv( iSockMover, pcc, iBuf, 0 )) < 0 )
      {
         if (iDebug)
         {
            printf("-E- receiving data from data mover (buffer %d)\n",
                   iBufnoServ);
            perror("EEE ");
         }
         goto gRetryData;
      }

      if (iDebug == 2)
      {
         printf("%d:", iRC);
         fflush(stdout);
      }

      if (iRC == 0)               /* connection closed by data mover */
      {
         printf("-E- data transfer incomplete\n");
         return -1;
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

/********************************************************************
 * rfio_write: write remote file in mass store managed by tsmcli
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_write(int iFileId,                              /* file id */
               char *pcBuffer,                       /* data buffer */
               int iItems)                  /* number of data bytes */
{
   char cModule[32] = "rfio_write";
   int iDebug = 0;

   int iRC;
   char cMsg[128] = "";
   char *pcc;
   int ii = 0;
   int iBuf;
   int *pint;

   int iMassSto = 0;   /* =1: contact GSI mass storage (first query)
                          =0: contact RFIO read/write server         */
   int iSockMover;                 /* socket for connection to mover */
   int iBufsizeWrite;
   int iBufnoServ;       /* no. of buffers transfered from/to server */

   srawAPIFile *pcurAPIFile;

   srawComm *pCommAPI;
   srawComm *pCommServ;            /* copy for servers in net format */

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   if (iItems <= 0)
   {
      printf("-W- invalid no. of bytes to write: %d\n", iItems);
      return 0;
   }

   if (iFileCount == 0)
   {
      printf("-E- no remote file open for write\n");
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
      printf("-E- remote file %d cannot be written: not opened\n",
             iFileId);
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

   pCommServ->iBufsizeFile = htonl(iItems);
   pCommAPI->iBufsizeFile = iItems;
   iBufsizeWrite = pcurAPIFile->iBufsizeAct;
   if (iDebug)
   {
      if (iBufsizeWrite != iItems)
         printf( "    new buffer size %d\n", iItems);
      printf("    send new buffer\n");
   }

   pcurAPIFile->iBufsizeAct = iItems;          /* keep current value */
   iSockMover = pcurAPIFile->iSockMover;

   /*********************** write next buffer ************************/

   iBufsizeWrite = htonl(iItems);
   iBuf = iint;
   pcc = (char *) &iBufsizeWrite;
   while (iBuf)
   {
      iRC = send( iSockMover, pcc, iBuf, 0 );
      if (iRC <= 0)
      {
         if (iRC == 0)
            printf("-E- send of data length to server broken\n");
         else
         {
            printf("-E- sending data length to server (buffer %d)",
                   iBufnoServ);
            perror("EEE ");
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

   iBuf = iItems;
   pcc = pcBuffer;
   while (iBuf)
   {
      iRC = send( iSockMover, pcc, iBuf, 0 );
      if (iRC <= 0)
      {
         if (iRC == 0)
            printf("-E- send of data to server broken\n");
         else
         {
            printf("-E- sending data to server (buffer %d, %d bytes)",
                   iBufnoServ, iBuf);
            perror("EEE ");
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

   return iItems;

} /* rfio_write */

/********************************************************************
 * rfio_lseek: in remote file in mass store managed by tsmcli:
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
         printf("-E- unexpected offset (%d)\n", ilocSeekMode);
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
         printf("-E- sending request for seek\n");
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
 * rfio_fopen: open remote file in mass store managed by tsmcli
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

RFILE *rfio_fopen(char *pcFile,                       /* file name */
                 char *pcOptions)                       /* options */
{
   char cModule[32] = "rfio_fopen";
   int iDebug = 0;
   int iRC;
   int iFileId;           /* file identifier for server connection */

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   if (*pcOptions == 'r')
   {
      iFileId = rfio_open(pcFile, O_RDONLY, 0);
   }
   if (*pcOptions == 'w')
   {
      iFileId = rfio_open(pcFile, O_WRONLY | O_CREAT, 0);
   }

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   if (iFileId < 0)
      return NULL;

   return pAPIFile[iFileId];        /* global, filled in rfio_open */

} /* rfio_fopen */

/********************************************************************
 * rfio_fread: read remote file in mass store managed by tsmcli
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_fread(char *pcBuffer,                      /* data buffer */
              int iSize,                      /* size of data item */
              int iItems,                  /* number of data items */
              RFILE *pRemFile)        /* remote file control block */
{
   char cModule[32]="rfio_fread";
   int iDebug = 0;
   int iRC;
   int iFileId;           /* file identifier for server connection */
   int iBytes;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   if (pRemFile == NULL)
   {
      printf("-E- Remote file not open for read\n");
      return -1;
   }

   iFileId = pRemFile->iFileId;
   iBytes = iSize*iItems;
   iRC = rfio_read(iFileId, pcBuffer, iBytes);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return iRC;

} /* rfio_fread */

/********************************************************************
 * rfio_fwrite: write remote file to mass store managed by tsmcli
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_fwrite(char *pcBuffer,                      /* data buffer */
              int iSize,                       /* size of data item */
              int iItems,                   /* number of data items */
              RFILE *pRemFile)         /* remote file control block */
{
   char cModule[32]="rfio_fwrite";
   int iDebug = 0;
   int iFileId;           /* file identifier for server connection */
   int iBytes;
   int iRC;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (pRemFile == NULL)
   {
      printf("-E- Remote file not open for write\n");
      return -1;
   }

   iFileId = pRemFile->iFileId;
   iBytes = iSize*iItems;
   iRC = rfio_write(iFileId, pcBuffer, iBytes);

   if (iDebug)
      printf("-D- end %s\n", cModule);

   return iRC;

} /* rfio_fwrite */

/********************************************************************
 * rfio_fclose: close remote file in mass store managed by tsmcli
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
   int iStatus = STA_END_OF_SESSION;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   iFileId = pRemFile->iFileId;
   iRC = rfio_close(iFileId);

   return iRC;

} /* rfio_fclose */

/********************************************************************
 * rfio_access: for remote file in mass store managed by tsmcli:
 *   get access status
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_access(char *pcFile,                         /* file name */
                int iMode)
{
   char cModule[32] = "rfio_access";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_access */

/********************************************************************
 * rfio_fstat: for remote file in mass store managed by tsmcli:
 *   get file status
 *
 * created 26. 2.2001, Horst Goeringer
 ********************************************************************
 */

int rfio_fstat(int iFileId,                              /* file id */
               void* fBufStat) /* buffer with file statistics */
{
   char cModule[32] = "rfio_fstat";
   int iDebug = 0;
   int iRC = 0;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return iRC;

} /* rfio_fstat */

/********************************************************************
 * rfio_parse: for remote file in mass store managed by tsmcli:
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
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_parse */

/********************************************************************
 * rfio_perror: for remote file in mass store managed by tsmcli:
 *   print error message to stderr
 *
 * created 26. 2.2001, Horst Goeringer
 ********************************************************************
 */

void rfio_perror(const char *pUMsg)  /* user prefix to error message */
{
   char cModule[32] = "rfio_perror";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   if (strlen(rfio_errmsg) == 0)
      fprintf(stderr, "%s : No error message\n", pUMsg);
   else
      fprintf(stderr, "%s : %s\n", pUMsg, rfio_errmsg);

} /* rfio_perror */

/********************************************************************
 * rfio_serror: for remote file in mass store managed by tsmcli:
 *   print last RFIO related error message to string
 *
 * created 26. 2.2001, Horst Goeringer
 ********************************************************************
 */

char * rfio_serror()
{
   char cModule[32] = "rfio_serror";
   int iDebug = 0;
   int iRC;

   if (iDebug) printf(
      "-D- begin %s: errlen %d\n", cModule, strlen(rfio_errmsg));

   if (strlen(rfio_errmsg) == 0)
      return NULL;

   return rfio_errmsg;

} /* rfio_serror */

/********************************************************************
 * rfio_unlink: for remote file in mass store managed by tsmcli:
 *
 * created  4.12.2000, Horst Goeringer
 ********************************************************************
 */

int rfio_unlink(char *pcFile)                         /* file name */
{
   char cModule[32] = "rfio_unlink";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_unlink */

/********************************************************************
 * rfiosetopt: for remote file in mass store managed by tsmcli:
 *
 * created  9. 9.2002, Horst Goeringer
 ********************************************************************
 */

int rfiosetopt(int iopt, int *pival, int ilen)
{
   char cModule[32] = "rfiosetopt";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfiosetopt */

/********************************************************************
 * rfio_mkdir: for remote file in mass store managed by tsmcli:
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

int rfio_mkdir(const char *path, int mode)
{
   char cModule[32] = "rfio_mkdir";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_mkdir */

/********************************************************************
 * rfio_opendir: for remote file in mass store managed by tsmcli:
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

void *rfio_opendir(const char *dirpath)
{
   char cModule[32] = "rfio_opendir";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_opendir */

/********************************************************************
 * rfio_readdir: for remote file in mass store managed by tsmcli:
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

void *rfio_readdir(void *dirp)
{
   char cModule[32] = "rfio_readdir";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_readdir */

/********************************************************************
 * rfio_closedir: for remote file in mass store managed by tsmcli:
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

int rfio_closedir(void *dirp)
{
   char cModule[32] = "rfio_closedir";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_closedir */

/********************************************************************
 * rfio_stat: for remote file in mass store managed by tsmcli:
 *
 * created 28. 8.2003, Horst Goeringer
 ********************************************************************
 */

int rfio_stat(const char *path, struct stat *statbuf)
{
   char cModule[32] = "rfio_stat";
   int iDebug = 0;
   int iRC;

   if (iDebug)
      printf("-D- begin %s\n", cModule);

   return 0;

} /* rfio_stat */

/********************************************************************
 * rfio_gsi_query: query file information of opened remote file
 *
 * created  7. 2.2001, Horst Goeringer
 ********************************************************************
 */

int rfio_gsi_query(int iFileId,                          /* file id */
                   int iFull,                          /* verbosity */
                   char *pInfo)                      /* info string */
{
   char cModule[32] = "rfio_gsi_query";
   int iDebug = 0;
   int iRC = 0;
   int ii = 0;

   srawAPIFile *pcurAPIFile;
   srawObjAttr *pQueryAPI;                      /* query information */

   if (iFileCount == 0)
   {
      printf("-E- %s: no remote file available\n", cModule);
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
      printf("-E- %s: remote file %d not found\n",
             cModule, iFileId);
      return -1;
   }

   pcurAPIFile = pAPIFile[iFileId];
   pQueryAPI = &(pcurAPIFile->sQueryAPI);
   iRC = rawQueryString(pQueryAPI, iFull, pInfo);
   if (iDebug)
      printf("-D- %s (%d byte)\n", pInfo, iRC);

   return iRC;

} /* rfio_gsi_query */

