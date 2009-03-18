/**********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 28. 8.1996 by Horst Goeringer
 **********************************************************************
 * rawCliProcn.c:
 *    utility programs for GSI mass storage: clients only
 *    new version
 **********************************************************************
 * rawCheckFilelist: remove objects already archived from file list
 * rawCheckObjlist:  remove existing files from object and file list,
 *                   reorder according to restore parameter
 * rawDelFile:     delete single file in GSI mass storage
 * rawDelList:     delete list of files in GSI mass storage
 * rawGetFilelistEntries: get filelist entries from input file
 * rawGetWSInfo:   get workspace and pool info from master
 * rawQueryFile:   get query information for single file
 * rawQueryList:   get query information from list of files
 * rawScanObjbuf:  check if obj already available in obj buffer chain
 * ielpst:         AIX only: elapsed (wall-clock) time measurement
 **********************************************************************
 * 26. 3.1996, H.G.: rawCheckObjlist: work on ptr-linked object buffers
 * 21.11.1997, H.G.: rawDelFile: only warning (rc=1) if obj not found
 * 19.12.2000, H.G.: rawCheckObjlist: remove unused code, better doc
 * 21. 2.2001, H.G.: renamed file rawQueryFile.c -> rawProcQuery.c
 *                   function rawQueryList added
 * 18. 5.2001, H.G.: merge into new file rawcliproc.c:
 *                   rawProcQuery.c
 *                   rawDelFile.c
 *                   rawDelList (rawDelFilelist from rawproc.c)
 *                   rawTapeFile.c
 *                   ielpst.c
 * 20. 7.2001, H.G.: rawCheckObjlist: mover node in srawRetrList
 * 24. 8.2001, H.G.: rawCheckObjlist: stage status in srawRetrList
 * 24. 1.2002, H.G.: created from rawcliproc.c, ACTION DELETE -> REMOVE
 * 13. 2.2002, H.G.: rawDelFile: handle also stage file
 * 25. 4.2002, H.G.: move rawTapeFile to new file rawProcTape.c
 *  5. 8.2002, H.G.: rawQueryFile: handle QUERY_RETRIEVE, QUERY_STAGE
 * 13. 1.2003, H.G.: rawCheckFilelist: reset cFilell before calling
 *                   rawGetLLName
 * 31. 1.2003, H.G.: use rawdefn.h
 *  4. 2.2003, H.G.: rawDelList, obj list: srawArchList -> srawRetrList
 *                   rawCheckFilelist, obj list: change type from
 *                   srawArchList -> srawRetrList
 * 14. 2.2003, H.G.: rawDelList: handles list of data movers
 *                   rawCheckFilelist: new arg pcNodeMaster
 * 17. 2.2003, H.G.: rawCheckFilelist, rawCheckObjlist from rawprocn.c
 * 17. 2.2003, H.G.: rawCheckFilelist: return in filelist ptr
 * 22. 5.2003, H.G.: renamed to rawCliProcn.c, new rawGetFilelistEntries
 * 27. 6.2003, H.G.: new function rawGetWSInfo
 * 18. 7.2003, H.G.: handle action QUERY_RETRIEVE_API
 * 23. 2.2004, H.G.: rawCheckObjlist: StageFS, ArchiveFS in srawRetrList
 *  5. 3.2004, H.G.: rawCheckObjlist: cArchiveDate, cOwner in srawRetrList
 **********************************************************************
 */

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef Linux
#include <linux/in.h>
#endif

#include "rawapitd.h"
#include "rawapitd-gsi.h"
#include "rawcommn.h"
#include "rawdefn.h"
#include "rawclin.h"
#include "rawentn.h"
#include "rawapplcli.h"

extern FILE *fLogFile;
extern int *piEntryList;                /* for rawGetFilelistEntries */

static int iqueryMax = 0;       /* max no. of files per query buffer */
static int iQueryCount = 0;            /* overall no. of files found */
static char *pQueryBuffer;                /* pointer to query buffer */
static srawQueryResult *pQueryResult;
static srawObjAttr *pQueryObj, *pQueryObj0;

/********************************************************************
 * rawCheckFilelist: remove objects already archived from file list
 *    returns number of removed objects
 *
 * created 15. 3.96, Horst Goeringer
 * 25. 3.1996, H.G.: work on ptr-linked object buffers
 ********************************************************************
 */

int rawCheckFilelist(char **pcFileList, char **pcObjList,
                     char *pcNodeMaster)
{
   char cModule[32] = "rawCheckFilelist";
   int iDebug = 0;

   char cDelimiter[1] = "";
   char *pcfl, *pcol;
   int *pifl, *piol;
   int ifile0, ifile;
   int iobj0, iobj, iobjf = 0;

   srawArchList *psFile, *psFile0;        /* files to be archived */
   srawRetrList *psObj, *psObj0;      /* objects already archived */

   char cFilell[MAX_OBJ_LL];
   char *pcFilell;             /* temp obj ll name from file name */

   bool_t bLastBuf;
   int iobjBuf;
   int **piptr;               /* points to pointer to next buffer */
   int *pinext;               /* points to next buffer */

   int ii, jj, iif;
   int iRC, idel;

   pcfl = *pcFileList;
   pifl = (int *) pcfl;
   ifile0 = pifl[0];                   /* initial number of files */
   ifile = ifile0;
   psFile = (srawArchList *) ++pifl;  /* points now to first file */
   psFile0 = psFile;
   pifl--;                     /* points again to number of files */

   pcol = *pcObjList;
   piol = (int *) pcol;
   iobj0 = piol[0];                  /* number of archived objects */
   iobj = iobj0;
   psObj = (srawRetrList *) ++piol;  /* points now to first object */
   psObj0 = psObj;
   piol--;                    /* points again to number of objects */

   pcFilell = cFilell;

   if (iDebug)
   {
      fprintf(fLogFile,
         "\n-D- begin %s: initial %d files, %d objects (1st buffer)\n",
         cModule, ifile0, iobj0);
      fprintf(fLogFile, "    first file %s, first obj %s\n",
             (char *) psFile, (char *) psObj);
   }

   iRC = strncmp( (char *) psObj, pcObjDelim, 1);
   if (iRC)
   {
      iRC = strncmp( (char *) psObj, pcObjDelimAlt, 1);
      if (iRC)
      {
         fprintf(fLogFile,
                "-E- %s: invalid object delimiter %1s found\n",
                cModule, (char *) psObj);
         return -1;
      }
      else
         strcpy(cDelimiter, pcObjDelimAlt);
   }
   else
      strcpy(cDelimiter, pcObjDelim);

   for (ii = 1; ii <= ifile0; ii++)                     /* file loop */
   {
      psObj = psObj0;                          /* first object again */
      iobj = iobj0;                     /* first object number again */
      strcpy(cFilell, "");
      iRC = rawGetLLName( (char *) psFile, cDelimiter, cFilell);

      if (iDebug)
         fprintf(fLogFile, "DDD cFilell: |%s|\n", cFilell);

      bLastBuf = bFalse;
      iobjBuf = 1;
      if (iDebug > 1)
         fprintf(fLogFile, "\n*** buffer %d: %d objs\n", iobjBuf, iobj);

      while (!bLastBuf)
      {

         for (jj = 1; jj <= iobj; jj++)         /* object loop */
         {

            iRC = strcmp(cFilell, (char *) psObj);
            if (iRC == 0)                      /* file archived */
            {
              if (iDebug) fprintf(fLogFile,
                 "-E- %s-%d: file %s already archived at specified path\n",
                 cModule, ii, ++pcFilell);
              else
                 fprintf(fLogFile, "-E- file %s already archived at specified path on server %s\n",
                        ++pcFilell, pcNodeMaster);
               pcFilell--;

               strncpy( (char *) psFile, "\n", 1);
               ifile--;
               if (ifile == 0)
               {
                  if (iDebug) fprintf(fLogFile,
                      "-D- %s: last file, comparison finished\n",
                      cModule);
                  goto gEndCompare;
               }
               goto gNextFile;

            } /* iRC == 0: file already archived */

            psObj++;
         } /* object loop */

         piptr = (int **) psObj;
         if (*piptr == NULL)
         {
            bLastBuf = bTrue;
            if (iDebug > 1)
               fprintf(fLogFile, "    %d: last obj buffer\n", iobjBuf);
            if (iDebug)
               fprintf(fLogFile, "*** %d: %s (%s) to be archived\n",
                      ii, (char *) psFile, cFilell);
         }
         else
         {
            iobjBuf++;
            pinext = *piptr;
            iobj = *pinext++;
            psObj = (srawRetrList *) pinext;
            if (iDebug > 1) fprintf(fLogFile,
               "\n*** new buffer %d: %d objs, first obj %s|\n",
               iobjBuf, iobj, (char *) psObj);
         }

      } /* while (!bLastBuf) */

gNextFile:
      psFile++;
   } /* file loop */

gEndCompare:
   idel = ifile0-ifile;
   if (iDebug) fprintf(fLogFile,
      "-D- %s: %d of %d files removed: %d remaining\n",
      cModule, idel, ifile0, ifile);

   pifl[0] = ifile;
   *pcFileList = (char *) pifl;

   if ( (ifile > 0) && (idel > 0) )        /* compress filelist */
   {
      if (iDebug) fprintf(fLogFile,
         "-D- compress file list: search %d files\n", ifile);
      iif = 1;
      psFile = psFile0;
      for (ii = 1; ii <= ifile0; ii++)             /* file loop */
      {
         /* fprintf(fLogFile, "%d: %s\n", ii, (char *) psFile); */
         iRC = strncmp( (char *) psFile, "\n", 1);
         if (iRC != 0)                            /* file found */
         {
            if (iif == ifile0) goto gEndCompress;
            psFile0++;
            psFile++;
            iif++;
         }
         else
         {                                    /* compress action */
            for (jj = ii+1; jj <= ifile0; jj++)
            {
               psFile++;
               iif++;
               iRC = strncmp( (char *) psFile, "\n", 1);
               if (iRC == 0)
               {
                  if (iif == ifile0) goto gEndCompress;
               }
               else
               {
                  strcpy( (char *) psFile0, (char *) psFile);
                  if (iDebug > 1) fprintf(fLogFile,
                     "*** %d: %s\n", iif, (char *) psFile0);
                  if (iif == ifile0) goto gEndCompress;
                  psFile0++;
               }
            } /* for (jj...) */

         } /* compress action */
      } /* for (ii...) */
   } /* ifile > 0 && idel > 0 */

gEndCompress:

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);
   return idel ;

} /* rawCheckFilelist */

/********************************************************************
 * rawCheckObjlist:
 *    on input:
 *       FileComp: no. of valid files/objs
 *       pcObjList: chain of ptr-linked buffers with obj lists
 *       pcFileList: corresponding file list (one buffer)
 *    on output:
 *       pcFileList: compressed and sorted file list
 *       pcObjListall: corresponding compressed and sorted obj list
 *                     (one buffer)
 *    remove files/objs with zero length ll names from both lists,
 *    reorder remaining files/objs according to restore parameter
 *
 * created 18. 3.96, Horst Goeringer
 ********************************************************************
 */

int rawCheckObjlist(int iFileComp,
                    char **pcObjList,
                    char **pcFileList,
                    char **pcObjListall)
{
   int iDebug = 0;
   char cModule[32]="rawCheckObjlist";
   int iRC;

   char *pcfl, *pcol, *pcolall;
   int  *pifl, *piol;
   int ifile0;
   int iobj0, iobj;

   char *pcFileBuf, *pcObjBuf;
   int iFileBuf, iObjBuf;
   int  *piflcopy, *piolcopy;

   int iiObj, iiBuf;
   int **piptr;               /* points to pointer to next buffer */
   int *pinext;               /* points to next buffer */

   srawArchList *psFile, *psFile0;        /* files to be written   */

   srawRetrList *psObj, *psObj0;        /* objects to be retrieved */
   srawRetrList *psObjMin;       /* object with min restore number */

   srawArchList *psFileCopy;              /* files to be written   */
   srawRetrList *psObjCopy;             /* objects to be retrieved */

   int ii, jj, jjMin;
   int icount;

   unsigned int irestoHLmin, irestoLLmin;
   bool_t bInit;

   pcfl = *pcFileList;
   pifl = (int *) pcfl;
   ifile0 = pifl[0];                       /* number of all files */
   psFile = (srawArchList *) ++pifl;  /* points now to first file */
   psFile0 = psFile;
   pifl--;                     /* points again to number of files */

   pcolall = *pcObjListall;
   pcol = *pcObjList;
   piol = (int *) pcol;
   iobj0 = piol[0];        /* number of archived objects in buffer */
   psObj = (srawRetrList *) ++piol;  /* points now to first object */
   psObj0 = psObj;
   piol--;                    /* points again to number of objects */

   if (iDebug)
   {
      fprintf(fLogFile,
         "\n-D- begin %s: initial %d files, %d objects (1st buffer)\n",
         cModule, ifile0, iobj0);
      fprintf(fLogFile,
         "    compressed: %d files/objects\n", iFileComp);
   }
   if (iDebug > 1)
      fprintf(fLogFile, "    first file %s, first obj %s\n",
              (char *) psFile, psObj->cNamell);

   iFileBuf = sizeof(int) + iFileComp*sizeof(srawArchList);
   if ((pcFileBuf = (char *) calloc(iFileBuf, sizeof(char) ) ) == NULL)
   {
      perror("-E- rawCheckObjlist: allocating filelist buffer");
      return(-2);
   }
   if (iDebug) fprintf(fLogFile,
      "    filelist buffer allocated for retrieve (size %d)\n",
      iFileBuf);

   piflcopy = (int *) pcFileBuf;
   piflcopy[0] = iFileComp;           /* compressed number of files */
   psFileCopy = (srawArchList *) ++piflcopy;
                                      /* points now to first file */

   iObjBuf = sizeof(int) + iFileComp*sizeof(srawRetrList);
   if ( ( pcObjBuf = (char *) calloc(iObjBuf, sizeof(char) ) ) == NULL )
   {
      perror("-E- rawCheckObjlist: allocating objlist buffer");
      return(-3);
   }
   if (iDebug) fprintf(fLogFile,
      "    objlist buffer allocated for retrieve (size %d)\n", iObjBuf);

   piolcopy = (int *) pcObjBuf;
   piolcopy[0] = iFileComp;           /* compressed number of files */
   psObjCopy = (srawRetrList *) ++piolcopy;
                                        /* points now to first file */

   iiObj = 0;                   /* no. of objects in current buffer */
   iiBuf = 1;                       /* no. of current object buffer */
   icount = 0;                      /* no. of files/objects copied  */
   for (ii = 1; ii <= iFileComp; ii++)
   {                   /* loop over COMPRESSED no. of files/objects */
      if (iDebug > 1)
         fprintf(fLogFile, "\nDDD object %d:\n", ii);
      bInit = bTrue;
      iobj = iobj0;         /* no. of arch. objects in first buffer */

      for (jj = 1; jj <= ifile0; jj++)
      {                              /* loop over ALL files/objects */
         iiObj++;
         if (iDebug > 1) fprintf(fLogFile,
            "*** buffer %d, obj %d: %s\n",
            iiBuf, iiObj, psObj->cNamell);
         iRC = strncmp(psObj->cNamell, "\0", 1);
         if (iDebug > 1) fprintf(fLogFile,
            "DDD %d,%d: %s, iRC %d\n", ii, jj, psObj->cNamell, iRC);
         if (iRC != 0)
         {
            if (bInit)
            {
               jjMin = jj;
               psObjMin  = psObj;            /* default min: 1st obj */
               irestoHLmin = psObj->iRestoHigh;
               irestoLLmin = psObj->iRestoLow;
               bInit = bFalse;
               if (iDebug > 1) fprintf(fLogFile,
                  "DDD0 new min %s: %d-%d\n", (char *) psObjMin,
                  irestoHLmin, irestoLLmin);
               if (ii == iFileComp)             /* last file/object */
                  goto gStore;
            }
            if (iDebug > 1) fprintf(fLogFile,
               "DDD found: %d-%d\n",
               psObj->iRestoHigh, psObj->iRestoLow);
            if (psObj->iRestoHigh < irestoHLmin)
            {
               jjMin = jj;
               psObjMin  = psObj;                       /* keep obj */
               irestoHLmin = psObj->iRestoHigh;
               irestoLLmin = psObj->iRestoLow;
               if (iDebug > 1) fprintf(fLogFile,
                  "DDD1 new min %s: %d-%d\n", (char *) psObjMin,
                  irestoHLmin,
                  irestoLLmin);
               if (ii == iFileComp)             /* last file/object */
                  goto gStore;
            }
            else
            {
               if (psObj->iRestoHigh == irestoHLmin)
               {
                  if (psObj->iRestoLow < irestoLLmin)
                  {
                     jjMin = jj;
                     psObjMin  = psObj;                /* keep obj */
                     irestoHLmin = psObj->iRestoHigh;
                     irestoLLmin = psObj->iRestoLow;
                     if (iDebug > 1) fprintf(fLogFile,
                        "DDD2 new min %s: %d-%d\n",
                        (char *) psObjMin,
                        irestoHLmin,
                        irestoLLmin);
                     if (ii == iFileComp)      /* last file/object */
                        goto gStore;

                  } /* psObj->iRestoLow < irestoLLmin */
               } /* psObj->iRestoHigh == irestoHLmin */

            } /* psObj->iRestoHigh >= irestoHLmin */
         } /* iRC != 0 */

         psObj++;
         if (iiObj == iobj)
         {
            if (iDebug > 1) fprintf(fLogFile,
               "    buffer %d: last obj (no. %d) handled\n",
               iiBuf, iiObj);
            piptr = (int **) psObj;
            if (*piptr == NULL)
            {
               if (iDebug > 1) fprintf(fLogFile,
                  "    %d: last obj buffer\n", iiBuf);
            }
            else
            {
               iiObj = 0;
               iiBuf++;
               pinext = *piptr;
               iobj = *pinext++;
               psObj = (srawRetrList *) pinext;
               if (iDebug > 1) fprintf(fLogFile,
                  "    %d: new buffer, %d objs, first: %s|\n",
                  iiBuf, iobj, psObj->cNamell);
            }

         } /* iiObj == iobj */


      } /* for (jj...) */

gStore:
      icount++;
      psObj = psObjMin;
      if (iDebug)
      {
         fprintf(fLogFile, "-D- copy %d-%d\n",
                 irestoHLmin, irestoLLmin);
         fprintf(fLogFile, "    obj %s, %d-%d\n", psObj->cNamell,
                 psObj->iObjHigh, psObj->iObjLow);
      }

      /* write obj items to temp structure */
      strcpy(psObjCopy->cNamell, psObj->cNamell);
      psObjCopy->iObjHigh = psObj->iObjHigh;
      psObjCopy->iObjLow =  psObj->iObjLow;
      psObjCopy->iRestoHigh = psObj->iRestoHigh;
      psObjCopy->iRestoLow = psObj->iRestoLow;
      psObjCopy->iFileType = psObj->iFileType;
      psObjCopy->iBufsizeFile = psObj->iBufsizeFile;
      psObjCopy->iFileSize = psObj->iFileSize;
      psObjCopy->iStageFS = psObj->iStageFS;
      strcpy(psObjCopy->cNodeMover, psObj->cNodeMover);
      psObjCopy->iArchiveFS = psObj->iArchiveFS;
      strcpy(psObjCopy->cArchiveDate, psObj->cArchiveDate);
      strcpy(psObjCopy->cOwner, psObj->cOwner);

      strncpy(psObj->cNamell, "\0", 1);      /* make it invalid */

      psFile = psFile0;
      psFile += (jjMin-1);
      if (iDebug)
         fprintf(fLogFile, "    file %s\n", (char *) psFile->cFile);
      iRC = strncmp(psFile->cFile, "\0", 1);
      if (iRC == 0)
      {
         fprintf(fLogFile,
            "-E- %s: file and object lists inconsistent\n", cModule);
         return(-4);
      }
      else if (iDebug)
         fprintf(fLogFile, "    write file %s to temp structure\n",
                 (char *) psFile->cFile);

      /* write file to temp structure */
      strcpy(psFileCopy->cFile, psFile->cFile);

      strncpy(psFile->cFile, "\0", 1);       /* make it invalid */

      /* set ptrs for next iteration */
      psObj = psObj0;
      psObjCopy++;
      psFile = psFile0;
      psFileCopy++;

      iiObj = 0;
      iiBuf = 1;

   } /* for (ii..) */

   if (icount != iFileComp)
   /* if (iRC != 0) */
   {
      fprintf(fLogFile,
         "-E- %s: unexpected no. of objects (%d) found\n",
         cModule, icount);
      return -5;
   }
   else if (iDebug)
      fprintf(fLogFile, "    obj no. %d okay\n", icount);

   /* memset(pcol, 0x00, iObjBuf); */
   memcpy(pcolall, pcObjBuf, iObjBuf);
   memset(pcfl, 0x00, iFileBuf);
   memcpy(pcfl, pcFileBuf, iFileBuf);

   if (iDebug) fprintf(fLogFile,
      "-D- end %s\n\n", cModule);
   return 0;

} /* rawCheckObjlist */

/*********************************************************************
 * rawDelFile: delete single file in GSI mass storage
 *
 * created  8.10.1996, Horst Goeringer
 *********************************************************************
 */

int rawDelFile( int iSocket,
                srawComm *pCommBuf)
{
   int iDebug = 0;
   char cModule[32] = "rawDelFile";

   int iStage = 0;
   int iRC;
   int iIdent;
   int iStatus, iStatusLen;
   int iBuf, iBufComm;
   char *pc, *pBuf;

   srawComm *pComm;
   srawStatus sStatus;
   srawQueryResult sQuery;
   srawObjAttr *pQAttr;

   pComm = pCommBuf;
   iBufComm = ntohl(pComm->iCommLen) + HEAD_LEN;
   if (iDebug) printf(
      "\n-D- begin %s: delete file %s\n", cModule, pComm->cNamell);

   /* make query to get object ids (unavailable in archive lists) */
   pBuf = (char *) &sQuery;
   pQAttr = &(sQuery.objAttr);
   pComm->iAction = htonl(QUERY_REMOVE);
   iRC = rawQueryFile(iSocket, pComm, (void *) &pBuf);

   if (iDebug)
      printf("    after rawQueryFile, rc = %d\n", iRC);

   if (iRC <= 0)
   {
      if (iRC < 0)
         return iRC;                                /* error occured */

      printf("-W- object %s%s%s not found\n",
             pComm->cNamefs, pComm->cNamehl, pComm->cNamell);
      return 1;
   }

   if ( (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_STAGE) ||
        (ntohl(pQAttr->iMediaClass) == GSI_MEDIA_LOCKED) )
      iStage = ntohl(pQAttr->iFS);
   else
      iStage = 0;
   pComm->iStageFSid = htonl(iStage);

   if (iDebug)
   {
      printf("    file %s found (obj-id %d-%d)",
             pComm->cNamell,
             ntohl(pQAttr->iObjHigh),
             ntohl(pQAttr->iObjLow));
      if (iStage)
         printf( ", on %s in StageFS %d\n", pQAttr->cNode, iStage);
      else
         printf( "\n");
   }

   pComm->iAction = htonl(REMOVE);
   pComm->iObjHigh = pQAttr->iObjHigh;
   pComm->iObjLow = pQAttr->iObjLow;

   pc = (char *) pComm;
   if ( (iRC = send( iSocket, pc, iBufComm, 0 )) < 0 )
   {
      perror("-E- sending command buffer");
      printf("-E- %s: sending delete request %s\n",
             cModule, pComm->cNamell);
      return -1;
   }

   if (iDebug) printf(
      "-D- delete command sent to server (%d bytes), look for reply\n",
      iBufComm);

   /*********** look for reply from server *****************/

   pc = (char *) &sStatus;
   iRC = rawRecvStatus(iSocket, &pc);
   if (iRC < HEAD_LEN)
   {
      printf("-E- receiving status buffer with delete information\n");
      return(-1);
   }
   if (iRC > HEAD_LEN)
   {
      printf("-E- message received from server:\n");
      printf("%s", sStatus.cStatus);
      return(-1);
   }

   if (iDebug)
      printf("    status (%d) received from server (%d bytes)\n",
             sStatus.iStatus, iRC);

   printf("-I- Object %s%s%s successfully deleted",
          pComm->cNamefs, pComm->cNamehl, pComm->cNamell);
   if (iStage)
      printf( " (staged)\n");
   else
      printf( "\n");

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return 0;

} /* end rawDelFile */

/*********************************************************************
 * rawDelList:  delete list of files in GSI mass storage
 *
 * created 22.10.1997, Horst Goeringer
 *********************************************************************
 */

int rawDelList( int iDataMover,
                srawDataMoverAttr *pDataMover0,
                srawComm *psComm,
                char **pcFileList,
                char **pcObjList)
{
   char cModule[32] = "rawDelList";
   int iDebug = 0;

   int iSocket;
   char *pcfl, *pcol;
   int *pifl, *piol;
   int ifile;
   int iobj0, iobj, iobjf = 0;
   int iobjBuf;

   srawArchList *psFile, *psFile0;           /* files to be archived */
   srawRetrList *psObj;                  /* objects already archived */
   srawDataMoverAttr *pDataMover;              /* current data mover */

   char cFilell[MAX_OBJ_LL];
   char *pc, *pdelim;

   bool_t bDelete, bDelDone;
   int **piptr;                  /* points to pointer to next buffer */

   int ii, jj, kk;
   int iRC, idel;

   pcfl = *pcFileList;
   pifl = (int *) pcfl;
   ifile = pifl[0];                            /* number of files */
   psFile = (srawArchList *) ++pifl;  /* points now to first file */
   psFile0 = psFile;
   pifl--;                     /* points again to number of files */

   if (iDebug)
   {
      printf("\n-D- begin %s\n", cModule);
      printf("    initial %d files, first file %s\n",
             ifile, psFile0->cFile);
   }

   pDataMover = pDataMover0;
   if (iDataMover == 1)
   {
      iSocket = pDataMover->iSocket;
      if (iDebug) printf(
         "    one data mover %s, socket %d\n",
         pDataMover->cNode, iSocket);
   }

   pcol = *pcObjList;
   piol = (int *) pcol;
   iobj0 = 0;                     /* total no. of archived objects */

   iobjBuf = 0;                             /* count query buffers */
   idel = 0;                                /* count deleted files */
   bDelDone = bFalse;
   while (!bDelDone)
   {
      iobjBuf++;
      iobj = piol[0];            /* no. of objects in query buffer */
      psObj = (srawRetrList *) ++piol;
                                     /* points now to first object */
      if (iDebug)
         printf("    buffer %d: %d objects, first obj %s (server %d)\n",
                iobjBuf, iobj, psObj->cNamell, psObj->iMaster);

      psComm->iAction = htonl(REMOVE);
      for (ii=1; ii<=iobj; ii++)    /* loop over objects in buffer */
      {
         iobj0++;
         pc = (char *) psObj;
         pc++;                           /* skip object delimiter  */

         bDelete = bFalse;
         psFile = psFile0;
         for (jj=1; jj<=ifile; jj++)                  /* file loop */
         {
            pdelim = strrchr(psFile->cFile, *pcFileDelim);
            if (pdelim == NULL)
            {
#ifdef VMS
               pdelim = strrchr(psFile->cFile, *pcFileDelim2);
               if (pdelim != NULL) pdelim++;
               else
#endif
               pdelim = psFile->cFile;
            }
            else pdelim++;                 /* skip file delimiter  */

            iRC = strcmp(pdelim, pc);
            if ( iRC == 0 )
            {
               bDelete = bTrue;
               break;
            }
            psFile++;
         } /* file loop (jj) */

         if (bDelete)
         {
            if (iDebug)
            {
               printf("    found file %d: %s, obj %d: %s",
                      jj, psFile->cFile, ii, psObj->cNamell);
               if (psObj->iStageFS)
                  printf(", on DM %s in StageFS %s\n",
                         psObj->cNodeMover, psObj->iStageFS);
               else if (psObj->iArchiveFS)
               {
                  printf(", on DM %s in ArchiveFS %s\n",
                         psObj->cNodeMover, psObj->iArchiveFS);
                  printf("    archived at %s by %s\n",
                         psObj->cArchiveDate, psObj->cOwner);
               }
               else
                  printf(" (not in disk pool)\n");
            }

            iRC = rawGetLLName(psFile->cFile,
                                pcObjDelim, psComm->cNamell);

            if (iDataMover > 1)
            {
               if ((strcmp(pDataMover->cNode,psObj->cNodeMover) == 0) ||
                   (strlen(psObj->cNodeMover) == 0))   /* not staged */
               {
                  iSocket = pDataMover->iSocket;
                  if (iDebug) printf(
                     "    current data mover %s, socket %d\n",
                     pDataMover->cNode, iSocket);
               }
               else
               {
                  pDataMover = pDataMover0;
                  for (kk=1; kk<=iDataMover; kk++)
                  {
                     if (strcmp(pDataMover->cNode,
                                psObj->cNodeMover) == 0)
                        break;
                     pDataMover++;
                  }

                  if (kk > iDataMover)
                  {
                     printf("-E- %s: data mover %s not found in list\n",
                            cModule, psObj->cNodeMover);
                     return -1;
                  }

                  iSocket = pDataMover->iSocket;
                  if (iDebug) printf(
                     "    new data mover %s, socket %d\n",
                     pDataMover->cNode, iSocket);
               }
            } /* (iDataMover > 1) */

            iRC = rawDelFile(iSocket, psComm);
            if (iRC)
            {
               if (iDebug)
                  printf("    rawDelFile: rc = %d\n", iRC);
               if (iRC < 0)
               {
                  printf("-E- %s: file %s could not be deleted\n",
                         cModule, psFile->cFile);
                  return -1;
               }
               /* else: object not found, ignore */

            } /* (iRC) */
            idel++;

         } /* if (bDelete) */
         psObj++;

      } /* loop over objects in query buffer (ii) */

      piptr = (int **) psObj;
      if (*piptr == NULL) bDelDone = bTrue;
      else piol = *piptr;

   } /* while (!bDelDone) */

   if (iDebug) printf("-D- end %s\n\n", cModule);

   return(idel);

} /* rawDelList */

/**********************************************************************
 * rawGetFilelistEntries:
 *    get filelist entries from input file
 *    uses external buffer ptr piEntryList provided by caller
 *    removes tape file specifications
 *    replaces '%' (historical) by '?' (supported by TSM)
 *    removes tape file specifications
 *    removes duplicate entries
 *    orders: generic file names first
 *
 * created 22. 5.2003 by Horst Goeringer
 **********************************************************************
 */

int rawGetFilelistEntries( char *pcFileName,
                           int *piEntries)
{
   char cModule[32] = "rawGetFilelistEntries";
   int iDebug = 0;

   int ii, jj;
   int imax = MAX_FILE;
   char cEntry[MAX_FILE] = "", *pcEntry;
   char cFileName[MAX_FILE] = "";
   char cQualifier[16] = ".filelist";
   char *pcLeading = "@";
   char *pcc;
   char *ploc;

   int iRemove;
   int iGeneric = 0;                 /* = 1: generic file name found */
   int iSingle = 0;   /* = 1: file name without wildcard chars found */
   int iRC;
   int iError;

   int iGenericEntries = 0;
   int iSingleEntries = 0;
   int iEntries = -1;
   FILE *fiFile = NULL;

   int iMaxEntries;
   int iSizeBuffer;
   int *piFilelist, *piFilelisto;
   srawArchList *pFilelist,                  /* first filelist entry */
                *pFilelistc,               /* current filelist entry */
                *pFilelistco,   /* current filelist entry old buffer */
                *pFilelistc0;   /* current filelist entry for remove */

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   piFilelist = piEntryList;             /* copy of external pointer */

   if (strlen(pcFileName) >= imax)
   {
      fprintf(fLogFile,
         "-E- %s: file name too long (max %d)\n",
         cModule, --imax);
      iError = -1;
      goto gErrorFilelist;
   }
   strcpy(cFileName, pcFileName);                      /* local copy */

   iRC = strncmp(cFileName, pcLeading, 1);
   if (iRC)
   {
      if (iDebug) fprintf(fLogFile,
         "    no leading %s\n", pcLeading);
      goto gEndFilelist;
   }

   pcc = cQualifier;
   ploc = strrchr(cFileName, pcc[0]);
   if (ploc != NULL)
   {
      ploc++;
      pcc++;
      if (strcmp(ploc, pcc) != 0)
      {
         if (iDebug) fprintf(fLogFile,
            "    no trailing %s\n", cQualifier);
         goto gEndFilelist;
      }
   }
   else
   {
      if (iDebug) fprintf(fLogFile,
         "    no trailing %s\n", cQualifier);
      goto gEndFilelist;
   }

   if (iDebug) fprintf(fLogFile,
      "    %s is a filelist\n", cFileName);

   fiFile = fopen(pcFileName, "r");
   if (fiFile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening filelist %s\n",
              cModule, pcFileName);
      perror("    ");
      iError = -1;
      goto gErrorFilelist;
   }
   if (iDebug) fprintf(fLogFile,
      "    filelist %s opened\n", pcFileName);

   piFilelisto = piFilelist;
   iMaxEntries = *piFilelist;
   if (iDebug) fprintf(fLogFile,
      "    max no. of entries in filelist buffer: %d\n", iMaxEntries);
   if (iDebug == 2) fprintf(fLogFile,
      "DDD piFilelist %d, *piFilelist %d\n",
      piFilelist, *piFilelist);

   pFilelist = (srawArchList *) &(piFilelist[1]);
                                          /* skip max no. of entries */
   pFilelistc = pFilelist;                          /* current entry */
   pcEntry = cEntry;
   iEntries = 0;
   for(;;)
   {
      pcEntry = fgets(cEntry, imax, fiFile);

      if ( (pcEntry != NULL) && (strlen(pcEntry) > 1) )
      {
         ploc = strchr(pcEntry, '\n');
         if (ploc != NULL)
            *ploc = '\0';

         ploc = strchr(pcEntry, *pcDevDelim);
         if (ploc != NULL)
         {
            fprintf(fLogFile,
               "-W- no local tape support in filelist, %s ignored\n",
               pcEntry);
            continue;
         }

         /* replace '%' (historical) by '?' (supported by TSM) */
         while ( (ploc = strchr(pcEntry, *pcPerc)) != NULL)
         {
            if (iDebug)
               fprintf(fLogFile, "-W- replace %s", pcEntry);
            *ploc = *pcQM;
            if (iDebug)
               fprintf(fLogFile, " by %s\n", pcEntry);
         }

         if (iEntries == iMaxEntries)    /* reallocate for new entry */
         {
            iMaxEntries += iMaxEntries;
            if (iDebug) fprintf(fLogFile,
               "    entry buffer full, reallocate: max %d entries\n",
               iMaxEntries);

            iSizeBuffer = iMaxEntries*MAX_FILE + sizeof(int);
            piFilelisto = piFilelist;
            piFilelist = (int *) calloc(iSizeBuffer, sizeof(char) );
            if (piFilelist == NULL)
            {
               fprintf(fLogFile,
                  "-E- reallocating filelist buffer (size %d)\n",
                  iSizeBuffer);
               perror("-E- reallocating filelist buffer");
               iError = -1;
               goto gErrorFilelist;
            }

            if (iDebug) fprintf(fLogFile,
               "    filelist entry buffer reallocated (size %d)\n",
               iSizeBuffer);

            *piFilelist = iMaxEntries;
            if (iDebug == 2) fprintf(fLogFile,
               "DDD piFilelist %d, *piFilelist %d\n",
                piFilelist, *piFilelist);

            pFilelistc = (srawArchList *) &(piFilelist[1]);   /* new */
            pFilelistco = pFilelist;                   /* old buffer */
            pFilelist = pFilelistc;              /* first new buffer */

            for (ii=1; ii<=iEntries; ii++)
            {
               if (iDebug == 2) fprintf(fLogFile,
                  "DDD pFilelistc %d\n", pFilelistc);
               strcpy(pFilelistc->cFile, pFilelistco->cFile);
               pFilelistc++;
               pFilelistco++;
            }

            if (iDebug) fprintf(fLogFile,
               "    %d old entries copied to new buffer, next:\n",
               iEntries);

            if (iDebug == 2) fprintf(fLogFile,
               "DDD free piFilelisto %d\n", piFilelisto);
            free(piFilelisto);
            piFilelisto = piFilelist;
         } /* (iEntries == iMaxEntries) */

         if (iDebug == 2) fprintf(fLogFile,
            "DDD pFilelistc %d\n", pFilelistc);
         strcpy(pFilelistc->cFile, pcEntry);
         iEntries++;

         if (iDebug) fprintf(fLogFile,
            "    %3d: %s \n", iEntries, pFilelistc->cFile);

         pFilelistc++;
      }
      else
         break;

   } /* read loop */

   pFilelistc = pFilelist;
   if (iDebug)
      for (ii=1; ii<=iEntries; ii++)
   {
      fprintf(fLogFile,
         "    %3da: %s \n", ii, pFilelistc->cFile);
      pFilelistc++;
   }

   /* check if duplicate entries */
   if (iEntries)
   {
      pFilelistc = pFilelist;
      iRemove = 0;
      for (ii=1; ii<iEntries; ii++)
      {
         if (strlen(pFilelistc->cFile) == 0)      /* already removed */
         {
            pFilelistc++;
            continue;
         }

         pFilelistco = pFilelistc;
         pFilelistco++;
         for (jj=ii+1; jj<=iEntries; jj++)
         {
            if (strcmp(pFilelistc->cFile, pFilelistco->cFile) == 0)
            {
               if (iDebug)
                  fprintf(fLogFile, "-W- duplicate entry %s removed\n",
                          pFilelistco->cFile);
               strcpy(pFilelistco->cFile, "");
               iRemove++;
            }
            pFilelistco++;
         }

         pFilelistc++;
      }

      if (iDebug)
      {
         if (iRemove) fprintf(fLogFile,
            "    %d duplicate entries found in list\n", iRemove);
         else fprintf(fLogFile,
            "    no duplicate entries found in list\n");
      }
   } /* (iEntries) */
   else if (iDebug)
      fprintf(fLogFile, "    no entries found in list\n");

   pFilelistc = pFilelist;
   if (iDebug)
      for (ii=1; ii<=iEntries; ii++)
   {
      fprintf(fLogFile,
         "    %3db: %s \n", ii, pFilelistc->cFile);
      pFilelistc++;
   }

   /* reorder remaining entries */
   if (iRemove)
   {
      pFilelistc = pFilelist;
      for (ii=1; ii<iEntries; ii++)
      {
         if (strlen(pFilelistc->cFile) == 0)              /* removed */
         {
            pFilelistco = pFilelistc;
            pFilelistco++;
            pFilelistc0 = pFilelistc;
            for (jj=ii+1; jj<=iEntries; jj++)
            {
               strcpy(pFilelistc0->cFile, pFilelistco->cFile);
               pFilelistc0++;
               pFilelistco++;
            }
            continue;
                /* no ptr incr, as same location to be checked again */
         }
         pFilelistc++;
      }
      iEntries -= iRemove;
   }

   /* check if sorting necessary */
   pFilelistc = pFilelist;
   for (ii=1; ii<=iEntries; ii++)
   {
      ploc = strchr(pFilelistc->cFile, *pcStar);
      if (ploc != NULL)
         iGeneric = 1;
      else
      {
         ploc = strchr(pFilelistc->cFile, *pcQM);
         if (ploc != NULL)
            iGeneric = 1;
         else
         {
            ploc = strchr(pFilelistc->cFile, *pcPerc);
            if (ploc != NULL)
               iGeneric = 1;
            else
               iSingle = 1;
         }
      }
      if ( (iGeneric) && (iSingle) )
         break;

      pFilelistc++;
   }

   pFilelistc = pFilelist;
   if (iDebug)
      for (ii=1; ii<=iEntries; ii++)
   {
      fprintf(fLogFile,
         "    %3dc: %s \n", ii, pFilelistc->cFile);
      pFilelistc++;
   }

   /* reorder: generic names first */
   if ( (iGeneric) && (iSingle) )
   {
      piFilelisto = piFilelist;

      /* create buffer for sorted entries */
      iSizeBuffer = iEntries*MAX_FILE + sizeof(int);
      piFilelist = (int *) calloc(iSizeBuffer, sizeof(char) );
      if (piFilelist == NULL)
      {
         fprintf(fLogFile,
            "-E- allocating ordered filelist buffer (size %d)\n",
            iSizeBuffer);
         perror("-E- allocating ordered filelist buffer");
         iError = -1;
         goto gErrorFilelist;
      }

      if (iDebug) fprintf(fLogFile,
         "    ordered filelist buffer allocated (size %d)\n",
         iSizeBuffer);

      *piFilelist = iEntries;
      if (iDebug == 2) fprintf(fLogFile,
         "DDD piFilelist %d, *piFilelist %d\n",
         piFilelist, *piFilelist);

      pFilelistc = (srawArchList *) &(piFilelist[1]);
      pFilelist = pFilelistc;
      pFilelistco = (srawArchList *) &(piFilelisto[1]);
      iGenericEntries = 0;

      /* copy generic entries */
      for (ii=1; ii<=iEntries; ii++)
      {
         iGeneric = 0;
         iSingle = 0;
         ploc = strchr(pFilelistco->cFile, *pcStar);
         if (ploc != NULL)
            iGeneric = 1;
         else
         {
            ploc = strchr(pFilelistco->cFile, *pcQM);
            if (ploc != NULL)
               iGeneric = 1;
            else
            {
               ploc = strchr(pFilelistco->cFile, *pcPerc);
               if (ploc != NULL)
                  iGeneric = 1;
               else
                  iSingle = 1;
            }
         }

         if (iGeneric)
         {
            strcpy(pFilelistc->cFile, pFilelistco->cFile);
            strcpy(pFilelistco->cFile, "");
            pFilelistc++;
            iGenericEntries++;
         }
         pFilelistco++;

      } /* loop pFilelistco */

      pFilelistco = (srawArchList *) &(piFilelisto[1]);
      iSingleEntries = 0;

      /* copy non-generic entries */
      for (ii=1; ii<=iEntries; ii++)
      {
         if (strlen(pFilelistco->cFile) != 0)
         {
            strcpy(pFilelistc->cFile, pFilelistco->cFile);
            pFilelistc++;
            iSingleEntries++;
         }
         pFilelistco++;
      }

      if (iDebug) fprintf(fLogFile,
         "    %d generic file names, followed by %d non-generic file names\n",
         iGenericEntries, iSingleEntries);

   } /* (iGeneric) && (iSingle) */

   pFilelistc = pFilelist;
   if (iDebug)
      for (ii=1; ii<=iEntries; ii++)
   {
      fprintf(fLogFile,
         "    %3dd: %s \n", ii, pFilelistc->cFile);
      pFilelistc++;
   }

gEndFilelist:
   *piEntries = iEntries;
   iError = 0;

   if (iDebug == 2) fprintf(fLogFile,
      "DDD piFilelist %d, *piFilelist %d\n", piFilelist, *piFilelist);

gErrorFilelist:
   if (fiFile)
   {
      iRC = fclose(fiFile);
      if (iRC)
      {
         fprintf(fLogFile, "-E- %s: closing filelist %s\n",
                 cModule, pcFileName);
         perror("    ");
      }
   }

   piEntryList = piFilelist;             /* pass to external pointer */

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return iError;

} /* rawGetFilelistEntries */

/********************************************************************
 * rawGetWSInfo: get workspace and pool info from master server
 *
 * created 30. 6.2003 by Horst Goeringer
 ********************************************************************
 */

int rawGetWSInfo( srawCliActionComm *pCliActionComm,
                  srawPoolStatus *pPoolInfo,
                  srawWorkSpace **ppWorkSpace)
{
   int iDebug = 0;
   char cModule[32] = "rawGetWSInfo";
   int iRC;
   int iSocket;
   int iPoolInfo;               /* =1: provide info on stage pool(s) */
   int iPrintPoolInfo;            /* =1: print info on stage pool(s) */
   int iWorkSpaceInfo;              /* =1: provide info on workspace */

   int iAction;

   int iBuf;
   int ii;
   int iIdent;
   int iStatus;
   int iAttrLen;

   int iPoolId;            /* identifier of requested pool, = 0: all */
   int iPoolMode = 0;        /* =1: retrieve, =2: stage, =3: archive */
   int iSleepClean = 0;          /* sleep if clean job needs started */

   int iPoolmax;
   int iPoolcur;
   int iBufPool;

   char cPoolNameRand[32] = "RetrievePool";
   char cPoolNameTemp[32] = "StagePool";
   int iHardwareFree = 0;          /* free space in hardware (MByte) */
   int iPoolRandFree;           /* free space of random pool (MByte) */
   int iPoolRandUsed;           /* used space of random pool (MByte) */
   int iRandomExcess = 0;
          /* unused space of other pools used in random pool (MByte) */
   int iPoolTempAvail = 0;
   int iPoolTempMax;            /* overall size of temp pool (MByte) */
   int iPoolTempMaxWS;           /* max WS size in temp pool (MByte) */
   int iPoolTempFree;             /* free space of temp pool (MByte) */
   int iPoolTempUsed;             /* used space of temp pool (MByte) */
   int iPoolTempCheck;
                  /* min work space check size for temp pool (MByte) */
   int iTempSizeUnavail = 0;
               /* temp pool space currently allocated by other pools */

   int iWorkSizeNew;
              /* still to be staged for requested work space (MByte) */
   int iWorkSizeAll;         /* size of requested work space (MByte) */
   int iWorkFilesAll;            /* total no. of files in work space */
   int iWorkSizeSta = 0;  /* part of requ. work space already staged */
   int iWorkFilesSta = 0;
                        /* no. of files in work space already staged */
   int iWorkSizeStaTemp = 0;
                           /* part of staged work space in temp pool */
   int iWorkFilesStaTemp = 0;    /* no. of staged files in temp pool */
   int iWorkSizeStaRand = 0;
                         /* part of staged work space in random pool */
   int iWorkFilesStaRand = 0;  /* no. of staged files in random pool */
   int iWorkSizeEst = 0;  /* part of requ. work space size estimated */
   int iWorkFilesEst = 0;
                   /* no. of files in work space with size estimated */
   int iWorkSizeIncompl = 0;
                     /* part of work space not yet completely staged */
   int iWorkFilesIncompl = 0;
             /* no. of files in work space not yet completely staged */
   int iWorkStatus = 0;       /* status of work space sent by server */


   int *piBuffer;
   char *pcc;
   char cMsg[256] = "";
   char cMsgPref[8] = "";
   char cMisc[128] = "";
   char pcGenFile[MAX_FILE] = "";        /* full (generic) file name */

   srawWorkSpace *pWorkSpace;
   srawPoolStatusData *pPoolInfoData, *pPoolInfoData0;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   piBuffer = pCliActionComm->piBuffer;      /* area to receive data */
   iSocket = pCliActionComm->iSocket;
   iAction = pCliActionComm->iAction;

   iPoolInfo = 1;
   if (iAction == QUERY_POOL)
   {
      iPoolId = pCliActionComm->iStatus;
      iPrintPoolInfo = 1;
      iWorkSpaceInfo = 0;
   }
   else
   {
      strcpy(pcGenFile, pCliActionComm->pcFile);
      iPoolId = 2;
      if (pCliActionComm->iStatus)
         iPrintPoolInfo = 1;
      else
         iPrintPoolInfo = 0;
      iWorkSpaceInfo = 1;
   }

   if (iDebug)
   {
      printf("    action %d", iAction);
      if (iPrintPoolInfo)
         printf(", print pool info");
      if (iAction == QUERY_POOL)
         printf(", poolId  %d\n", iPoolId);
      else
         printf("\n");
   }

   if (iPoolInfo)
   {
      pcc = (char *) piBuffer;               /* header for pool info */
      pPoolInfo = (srawPoolStatus *) piBuffer;         /* for caller */
      iRC = rawRecvHead(iSocket, &pcc);
      if (iRC <= 0)
      {
        printf("-E- receiving header pool status from master server\n");
        return -1;
      }

      if (iDebug)
         printf("    header pool status received (%d bytes)\n", iRC);

      iIdent = ntohl(pPoolInfo->iIdent);
      iStatus = ntohl(pPoolInfo->iPoolNo);
      iAttrLen = ntohl(pPoolInfo->iStatusLen);

      /* expect IDENT_POOL */
      if (iIdent != IDENT_POOL)
      {
         if (iIdent == IDENT_STATUS)
         {
            if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) )
            {
               printf("-E- received error status from server");
               if (iDebug)
                  printf(" instead of pool info:\n");
               else printf(":\n");

               if (iAttrLen > 0)
               {
                  pcc = cMsg;
                  iRC = rawRecvError(iSocket, iAttrLen, &pcc);
                  if (iRC < 0) printf(
                     "-E- receiving error msg from server, rc = %d\n",
                     iRC);
                  else printf("    %s\n", pcc);
               }
               else printf("    no error message available\n");

            } /* (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) */
            else
            {
               printf(
                  "-E- unexpected status (type %d) received from server\n",
                  iStatus);
            }
         } /* iIdent == IDENT_STATUS */
         else
         {
            printf(
               "-E- unexpected header (%d) received from server\n",
               iIdent);
         }

         return -1;

      } /* (iIdent != IDENT_POOL) ) */

      iPoolmax = iStatus;
      iBufPool = iAttrLen;        /* size pool buffer without header */

      pcc += HEAD_LEN;
      pPoolInfoData = (srawPoolStatusData *) pcc;
      pPoolInfoData0 = pPoolInfoData;             /* first pool info */

      /* receive staging pool info */
      iBuf = iBufPool;
      while(iBuf > 0)
      {
         if ( (iRC = recv( iSocket, pcc, iBuf, 0 )) < 0 )
         {
            perror("-E- receiving pool info");
            return -1;
         }
         iBuf -= iRC;
         pcc += iRC;
      } /* while(iBuf > 0) */

      if (iDebug) printf(
         "    stage pool status received (%d bytes)\n", iBufPool);

      /* get pool names first for info messages */
      if ( (iAction == QUERY_POOL) || (iAction == QUERY_WORKSPACE) ||
           (iAction == STAGE) )
         for (iPoolcur=1; iPoolcur<=iPoolmax; iPoolcur++)
      {
         if (iPoolcur == 1)
         {
            iHardwareFree = ntohl(pPoolInfoData->iFreeSizeHW);
            iPoolRandFree = ntohl(pPoolInfoData->iFreeSize);
         }
         if (iPoolcur == 2)
            iPoolTempFree = ntohl(pPoolInfoData->iFreeSize);

         if (strcmp(pPoolInfoData->cPoolName, "RetrievePool") == 0)
            iPoolMode = 1;
         if (strcmp(pPoolInfoData->cPoolName, "StagePool") == 0)
            iPoolMode = 2;

         if ( (iAction == QUERY_POOL) || (iAction == QUERY_WORKSPACE) )
         {
            if (iPoolMode == 1)
            {
               if (iPrintPoolInfo) printf(
                  "-I- overall space %s %7d MByte, free %7d MByte\n",
                  pPoolInfoData->cPoolOS,
                  ntohl(pPoolInfoData->iMaxSizeHW), iHardwareFree);

               if ( (iPoolId == iPoolMode) || (iPoolId == 0) )
               {
                  printf("    %s: used for 'tsmcli retrieve'\n",
                     pPoolInfoData->cPoolName);

                  iPoolRandUsed = ntohl(pPoolInfoData->iMaxSize) -
                                  iHardwareFree;
                  if (iPoolRandFree < 0)
                     iRandomExcess = -iPoolRandFree;

                  printf(
                     "          used space  %7d MByte, free %7d MByte, %d files stored\n",
                     iPoolRandUsed, iHardwareFree,
                     ntohl(pPoolInfoData->iFiles));

                  if (iAction == QUERY_POOL)
                  {
                     if (iRandomExcess > 0)
                        printf("       %7d MByte lent from StagePool\n",
                               iRandomExcess);
                     else if (iHardwareFree > 0)
                        printf("       unused space also usable for StagePool\n");
                  }

               } /* (iPoolId == iPoolMode) || (iPoolId == 0) */
            } /* (iPoolMode == 1) */

            if (iPoolMode == 2)
            {
               if ( (iPoolId == iPoolMode) || (iPoolId == 0) )
               {
                  if (iPrintPoolInfo) printf(
                     "    %s: used for 'tsmcli stage'\n",
                     pPoolInfoData->cPoolName);
                  iPoolTempAvail = ntohl(pPoolInfoData->iFileAvail);

                  iPoolTempUsed = ntohl(pPoolInfoData->iMaxSize) -
                                  iPoolTempFree;
                  if (iPoolTempFree < 0)
                     iPoolTempFree = 0;

                  if (iHardwareFree < iPoolTempFree)
                     iTempSizeUnavail = iPoolTempFree - iHardwareFree;

                  if ( (iPoolTempFree >= 0) && (iPrintPoolInfo) )
                  {
                     printf( "          used space  %7d MByte, free %7d MByte, %d files stored\n",
                        iPoolTempUsed, iPoolTempFree,
                        ntohl(pPoolInfoData->iFiles));
                     if (iTempSizeUnavail)
                     {
                        printf( "       thereof currently %7d MByte free, remainder can be freed\n",
                           iHardwareFree);
                     }
                     else
                        if ( (iAction == QUERY_POOL) &&
                             (iPoolTempFree > 0) )
                           printf("       unused space also usable for RetrievePool\n");
                  }
                  if (iPrintPoolInfo) printf(
                     "       min availability of %d days guaranteed\n",
                     ntohl(pPoolInfoData->iFileAvail));

               } /* (iPoolId == iPoolMode) || (iPoolId == 0) */
            } /* (iPoolMode == 2) */
         } /* (iAction == QUERY_POOL) || (iAction == QUERY_WORKSPACE) */

         if ( (iPoolMode == 2) &&
              ((iAction == STAGE) || (iAction == QUERY_WORKSPACE)) )
         {
            iPoolTempMax = ntohl(pPoolInfoData->iMaxSize);
            iPoolTempMaxWS = ntohl(pPoolInfoData->iMaxWorkSize);
            iPoolTempCheck = ntohl(pPoolInfoData->iCheckSize);
         }

         pPoolInfoData++;

      } /* loop pools */
      pPoolInfoData = pPoolInfoData0;              /* 1st pool again */

   } /* (iPoolInfo) */

   /********************* get work space info ************************/

   if (iWorkSpaceInfo)
   {
      pWorkSpace = (srawWorkSpace *) pcc;
      *ppWorkSpace = pWorkSpace;                       /* for caller */

      iRC = rawRecvHead(iSocket, &pcc);
      if (iRC <= 0)
      {
         printf("-E- receiving work space buffer header\n");
         return -1;
      }
      if (iDebug)
         printf("-D- header work space buffer received (%d bytes)\n",
                iRC);

      iIdent = ntohl(pWorkSpace->iIdent);
      iStatus = ntohl(pWorkSpace->iWorkId);
      iAttrLen = ntohl(pWorkSpace->iStatusLen);

      /* expect IDENT_WORKSPACE */
      if (iIdent != IDENT_WORKSPACE)
      {
         if (iIdent == IDENT_STATUS)
         {
            if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) )
            {
               if (iDebug) printf("\n");
               printf("-E- received error status from server");
               if (iDebug)
                  printf(" instead of work space info:\n");
               else printf(":\n");

               if (iAttrLen > 0)
               {
                  pcc = (char *) piBuffer;
                  iRC = rawRecvError(iSocket, iAttrLen, &pcc);
                  if (iRC < 0) printf(
                     "-E- receiving error msg from server, rc = %d\n",
                     iRC);
                  else printf("    %s\n", pcc);
               }
               else printf("    no error message available\n");

            } /* (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) */
            else
            {
               printf(
                  "-E- unexpected status (%d) received from server\n",
                  iStatus);
            }
         } /* iIdent == IDENT_WORKSPACE */
         else
         {
            printf(
               "-E- unexpected header (%d) received from server\n",
               iIdent);
         }

         return -1;

      } /* (iIdent != IDENT_WORKSPACE) ) */

      /* receive work space info */
      iBuf = iAttrLen;
      pcc += HEAD_LEN;
      while(iBuf > 0)
      {
         if ( (iRC = recv( iSocket, pcc, iBuf, 0 )) < 0 )
         {
            perror("-E- receiving work space info");
            return -1;
         }
         iBuf -= iRC;
         pcc += iRC;
      } /* while(iBuf > 0) */

      if (iDebug)
         printf("    remainder work space buffer received (%d byte)\n",
                iAttrLen);

      iWorkSizeAll = ntohl(pWorkSpace->iWorkSizeAll);
      iWorkFilesAll = ntohl(pWorkSpace->iWorkFilesAll);
      iWorkStatus = ntohl(pWorkSpace->iStatus);
      iWorkSizeSta = ntohl(pWorkSpace->iWorkSizeSta);
      iWorkFilesSta = ntohl(pWorkSpace->iWorkFilesSta);
      iWorkSizeStaTemp = ntohl(pWorkSpace->iWorkSizeStaTemp);
      iWorkFilesStaTemp = ntohl(pWorkSpace->iWorkFilesStaTemp);
      iWorkSizeEst = ntohl(pWorkSpace->iWorkSizeEst);
      iWorkFilesEst = ntohl(pWorkSpace->iWorkFilesEst);

      iWorkSizeStaRand = iWorkSizeSta - iWorkSizeStaTemp;
      iWorkFilesStaRand = iWorkFilesSta - iWorkFilesStaTemp;

      /* provide infos if stage of large workspace or if ws_query */
      if (iPoolInfo || (iAction == QUERY_WORKSPACE) || (iDebug) )
      {
         if (iDebug)
         {
            printf("    %d files, overall size %d MByte",
                   iWorkFilesAll, iWorkSizeAll);
            if (iWorkFilesEst)
               printf(", size estimated for %d files (%d MByte)\n",
                      iWorkFilesEst, iWorkSizeEst);
         }

         if (iWorkSizeSta == iWorkSizeAll)
         {
            if (iDebug)
            {
               printf(
                  "\n    all files already available on central disk\n");
               if ( (iPoolId == 2) &&
                    iWorkSizeStaRand &&
                    (iAction == QUERY_WORKSPACE) )
                  printf(
                     "    to get a guaranteed availability of %d days on disk pool use 'tsmcli stage'\n",
                     iPoolTempAvail);
            }
         }
         else if (iWorkSizeSta)
         {
            if (iDebug)
            {
               printf(
                  "\n    %d files already available on central disk (%d MByte)\n",
                  iWorkFilesSta, iWorkSizeSta);
               if (iWorkFilesStaTemp)
                  printf(
                     "    %d files already in %s (%d MByte)\n",
                     iWorkFilesStaTemp, cPoolNameTemp, iWorkSizeStaTemp);
               printf(
                  "    %d files still to be staged or currently being staged (%d MByte)\n",
                  iWorkFilesAll-iWorkFilesSta,
                  iWorkSizeAll - iWorkSizeSta);
            }
         }
         else if (iDebug)
            printf(", all to be staged\n");

         /* handle problems or limitations indicated by iWorkStatus:
            = 0: needed size ws available
            = 1: overall size ws > free size StagePool
            = 2: overall size ws > allowed size of ws
            = 3: needed size ws > size StagePool
            = 9: needed size ws > clean limit, but available: no sleep
            =10: needed size ws partially available,
                 sleep 10 min before start staging
            =20: needed size ws partially available,
                 sleep 20 min before start staging
         */
         if ( (iWorkStatus < 9) && (iWorkStatus != 0) )
         {
            if (iAction == QUERY_WORKSPACE)
               strcpy(cMsgPref, "-W-");
            else
               strcpy(cMsgPref, "-E-");
            printf("%s requested workspace '%s' cannot be staged:\n",
                   cMsgPref, pcGenFile);

            if (iWorkStatus < 0)
               printf("%s staging disk pool currently unavailable\n",
                      cMsgPref);
            else if (iWorkStatus == 1)
            {
               printf("    currently free in %s:   %d MByte\n",
                      cPoolNameTemp, iPoolTempFree);
               printf(
                  "    still needed:                  %d MByte (%d files)\n",
                  iWorkSizeAll-iWorkSizeSta,
                  iWorkFilesAll-iWorkFilesSta);
               if (iAction == QUERY_WORKSPACE)
                  printf( "-I- Check later again!\n");
               else printf(
                  "-I- Query work space status before retrying later!\n");
            }
            else if ( (iWorkStatus == 2) || (iWorkStatus == 3) )
            {
               if (iWorkStatus == 2) printf(
                  "%s max work space allowed in '%s' is currently limited to %d MByte\n",
                  cMsgPref, cPoolNameTemp, iPoolTempMaxWS);
               else printf(
                  "%s overall size of %s is limited to %d MByte\n",
                  cMsgPref, cPoolNameTemp, iPoolTempMax);
               printf(
                  "-I- Please reduce your work space requirements\n");
            }
            else if (iWorkStatus < 9)
               printf(
                  "-E- unexpected workspace status received from server (%d)\n",
                  iWorkStatus);

            if (iDebug)
               printf("-D- end %s\n\n", cModule);
            return -1;

         } /* (iWorkStatus < 9) && (iWorkStatus != 0) */

         if ( (iDebug) && (iWorkSizeSta != iWorkSizeAll) )
         {
            if (iAction == QUERY_WORKSPACE)
            {
               iWorkSizeNew = iWorkSizeAll - iWorkSizeSta;
               if (iRandomExcess)
               {
                  if (iWorkSizeNew > iHardwareFree)
                  {
                     printf("-D- currently unused in %s: %d MByte\n",
                            cPoolNameTemp, iPoolTempFree);
                     printf("    currently free in %s:   %d MByte, remainder temporarily used by other pools\n",
                            cPoolNameTemp, iHardwareFree);
                  }
                  else
                     printf("-D- currently free in %s:   %d MByte\n",
                            cPoolNameTemp, iHardwareFree);
               }
               else
                  printf("-D- currently free in %s:   %d MByte\n",
                         cPoolNameTemp, iPoolTempFree);
               printf(
                  "    to get all files on disk pool with a guaranteed availability of %d days:\n",
                  iPoolTempAvail);
               printf("    -> use 'tsmcli stage'\n");

            } /* (iAction == QUERY_WORKSPACE) */
        /*  else
               printf("-D- currently free in %s:   %d MByte\n",
                      cPoolNameTemp, iPoolTempFree);
         */
         } /* (iDebug) && (iWorkSizeSta != iWorkSizeAll) */

         /* server set clean request */
         if (iWorkStatus >= 9)
         {
            if (iDebug)
            {
               /* if (ntohl(pPoolInfoData->iFreeSize) >= 0)
                  does not work in Linux:
                  negative values are not recognized!            */
               ii = ntohl(pPoolInfoData0->iFreeSize);

               printf("-D- currently free (HW): %d MByte\n",
                      iHardwareFree);
               printf(
                  "    currently %d MByte unused in %s are allocated by other pools\n",
                  ii*(-1), cPoolNameTemp);
            }

            if (iAction == QUERY_WORKSPACE)
               strcpy(cMisc, "must be initiated (tsmcli stage)");
            else
               strcpy(cMisc, "is initiated");
            printf(
               "-I- a clean job %s to provide the requested space\n",
               cMisc);

            if (iWorkStatus > 9)
            {
               /* iSleepClean = iWorkStatus*60;     */     /* in sec */
               iSleepClean = iWorkStatus;
           /*  if (iDebug)
                  printf("-D- sleep for %ds\n", iSleepClean);
               sleep(iSleepClean);
            */
            }

         } /* (iWorkStatus >= 9) */

      } /* (iPoolInfo) || (iAction == QUERY_WORKSPACE) */
   } /* (iWorkSpaceInfo) */

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return 0;

} /* rawGetWSInfo */

/********************************************************************
 * rawQueryFile: get query information for single file
 *
 * used for tape input handled sequentially
 * buffers for communication and query info allocated in calling
 *    procedure
 ********************************************************************
 */

int rawQueryFile( int iSocket,
                  srawComm *pCommBuf,
                  void **pQueryBuf)
{
   int iDebug = 0;
   char cModule[32] = "rawQueryFile";
   int iRC;
   int iIdent, iquery = -1, iAttrLen;
   int iStatus, iStatusLen;
   int iBuf, iBufComm;
   int ii;
   char *pc;

   srawComm *pComm;
   srawQueryResult *pQuery;
   srawObjAttr *pObjAttr;

   pComm = pCommBuf;
   pQuery = (srawQueryResult *) (*pQueryBuf);

   iBufComm = ntohl(pComm->iCommLen) + HEAD_LEN;
   if (iDebug) printf(
      "\n-D- begin %s: query file %s, action %d\n",
      cModule, pComm->cNamell, ntohl(pComm->iAction));

   switch( ntohl(pComm->iAction) )
   {
      case QUERY:
      case QUERY_ARCHIVE:
      case QUERY_ARCHIVE_RECORD:
      case QUERY_ARCHIVE_MGR:
      case QUERY_REMOVE:
      case QUERY_REMOVE_MGR:
      case QUERY_RETRIEVE:
      case QUERY_RETRIEVE_API:
      case QUERY_STAGE:
         ;                          /* okay */
         break;
      default:
         printf("-E- %s: invalid action %d\n",
                cModule, ntohl(pComm->iAction));
         return -1;
   }

   pc = (char *) pComm;
   if ( (iRC = send( iSocket, pc, iBufComm, 0 )) < 0 )
   {
      printf("-E- %s: query file %s\n", cModule, pComm->cNamell);
      perror("    sending command buffer");
      return(-1);
   }

   if (iDebug) printf(
      "    query command sent to server (%d bytes), look for reply\n",
      iBufComm);

   /*********** look for reply from server *****************/

gNextReply:
   pc = (char *) pQuery;
   iRC = rawRecvHead(iSocket, &pc);
   if (iRC <= 0)
   {
      printf("-E- %s: receiving buffer header with query information\n",
             cModule);
      return(-1);
   }
   else if (iDebug)
      printf("    %d bytes received\n", iRC);

   pc += HEAD_LEN;
   iIdent = ntohl(pQuery->iIdent);
   iquery = ntohl(pQuery->iObjCount);
   iAttrLen = ntohl(pQuery->iAttrLen);

   if ( (iIdent != IDENT_QUERY) && (iIdent != IDENT_QUERY_ARCHDB) )
   {
      if (iIdent == IDENT_STATUS)
      {
         iStatus = iquery;
         iStatusLen = iAttrLen;
         if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) )
         {
            if (iDebug)
               printf("\n");
            printf("-E- %s: received error status from server",
                   cModule);
            if (iDebug)
               printf(" instead of query data:\n");
            else
               printf(":\n");

            if (iStatusLen > 0)
            {
               iRC = rawRecvError(iSocket, iStatusLen, &pc);
               if (iRC < 0) printf(
                  "-E- receiving error msg from server, rc = %d\n",
                  iRC);
               else printf("    %s\n", pc);
            }
            else printf("    no error message available\n");

         } /* iStatus == STA_ERROR || (iStatus == STA_ERROR_EOF) */
         else printf(
           "-E- %s: unexpected status (type %d) received from server\n",
           cModule, iStatus);

      } /* iIdent == IDENT_STATUS */
      else printf(
         "-E- %s: unexpected header (type %d) received from server\n",
         cModule, iIdent);

      return(-1);

   } /* iIdent != IDENT_QUERY  && != IDENT_QUERY_ARCHDB */

   if (iquery > 0)
   {
      for (ii=1; ii<=iquery; ii++)
      {
         pc = (char *) pQuery + HEAD_LEN;
         pObjAttr = (srawObjAttr *) pc;
         iBuf = iAttrLen;        /* only space for one object buffer */
         while(iBuf > 0)
         {
            if ( (iRC = recv( iSocket, pc, iBuf, 0 )) < 0 )
            {
               printf("-E- %s: receiving object buffer\n", cModule);
               perror("-E- receiving query results");
               return -1;
            }
            iBuf -= iRC;
            pc += iRC;
         } /* while(iBuf > 0) */

         if (iDebug)
         {
            if (ii == 1) printf(
               "    query buffer received (%d bytes)\n", iAttrLen);
            else printf(
               "    query buffer overlaid (%d bytes)\n", iAttrLen);
            printf("    cNamell %s, iObjLow %d",
                   pObjAttr->cNamell, ntohl(pObjAttr->iObjLow));
            if (iIdent == IDENT_QUERY)
               printf(" (TSM DB)\n");
            else
               printf(" (ArchivePool)\n");
         }
      } /* object loop */
   } /* (iquery > 0) */
   else if (iIdent == IDENT_QUERY)        /* nothing found in TSM DB */
         goto gNextReply;                   /* check for ArchivePool */

   switch (iquery)
   {
      case 0:
         if (iDebug)
            printf("    no matching object %s found\n", pComm->cNamell);
         break;
      case 1:
         if (iDebug)
            printf("    file %s already archived\n", pComm->cNamell);
         break;
      default:
         if ( (iquery > 1) && (strcmp(pComm->cOwner, "goeri") == 0) )
            printf("-W- %d versions of %s exist!\n",
                   iquery, pComm->cNamell);
         else
         {
            printf("-E- %s: invalid number %d of objects %s found\n",
                   cModule, iquery, pComm->cNamell);
            iquery = -1;
         }
   } /* switch (iquery) */

   if (iDebug) printf(
      "-D- end %s\n\n", cModule);

   return iquery;

} /* end rawQueryFile */

/********************************************************************
 * rawQueryList: get query information from list of files
 *
 * created 21. 2.2001 by Horst Goeringer
 ********************************************************************
 */

int rawQueryList( int iSocket,
    char *pFile,                       /* generic name of file list */
    int iFilenoReq,          /* no. of requ. file (1 .. n), init: 0 */
    int iFull,                                   /* verbosity level */
    char *pInfo) /* query info of requested file (init: first file) */
{
   int iDebug = 0;
   char cModule[32] = "rawQueryList";
   int iRC;
   int iIdent;
   int iAttrLen;

   int iFileno;                               /* no. of files found */
   int iquery = -1;         /* no. of files in current query buffer */
   int iqueryBuf = 0;              /* no. of query buffers received */

   int iBuffer;                       /* size query buffer in bytes */

   int iStatus, iStatusLen;
   int iBuf, iBufComm;

   int ii;
   char *pcc;
   bool_t bQDone = bFalse;

   char cNamefs[MAX_OBJ_FS] = "", *pcNamefs;      /* filespace name */
   char cNamehl[MAX_OBJ_HL] = "", *pcNamehl;     /* high level name */
   char cNamell[MAX_OBJ_LL] = "", *pcNamell;     /* low  level name */

   char cOwner[DSM_MAX_OWNER_LENGTH] = "";
   char cliNode[MAX_NODE] = "";

   int iCommSize = sizeof(srawComm); /* client/server communication */
   srawComm *pComm, sComm;

   pComm = &sComm;

   pcNamefs = cNamefs;
   pcNamehl = cNamehl;
   pcNamell = cNamell;

   if (iDebug)
      printf("\n-D- this is %s\n", cModule);

   pComm->iIdent = htonl(IDENT_COMM);
   pComm->iAction = htonl(QUERY);
   pComm->iCommLen = htonl(iCommSize - HEAD_LEN);
   pComm->iBufsizeFile = htonl(0);

   /******************** allocate query buffer **********************/

   if (iFilenoReq > 0)
   {
      if (pQueryBuffer == 0)
      {
         printf("-W- assume initial call for file list %s\n", pFile);
         iFilenoReq = 0;
      }
   }

   if (iFilenoReq <= 0)
   {
      iBuffer = MBUF_SOCK;
      iqueryMax = (iBuffer - HEAD_LEN)/sizeof(srawObjAttr);
      /* iqueryMax = 3;        DDD must be synchronized with server */

      if ( (pQueryBuffer =
           (char *) calloc(iBuffer, sizeof(char))) == NULL )
      {
         perror("-E- allocating data buffer");
         printf("-E- allocating data buffer (size %d)\n", iBuffer);
         return 1;
      }
      if (iDebug)
      {
         printf("-D- data buffer allocated (size %d)\n", iBuffer);
         printf("    max %d queries per buffer\n", iqueryMax);
      }
      pQueryResult = (srawQueryResult *) pQueryBuffer;
      pQueryObj = &(pQueryResult->objAttr);
      pQueryObj0 = pQueryObj;  /* keep ptr to attributes first file */

   } /* iFilenoReq <= 0 */
   else
   {
      if (iFilenoReq > iqueryMax)
      {
         printf("-E- max %d file infos kept, please restrict your filelist %s\n",
                iqueryMax, pFile);
         return -9;
      }

      pQueryObj = pQueryObj0;   /* set ptr to attributes first file */
      for (ii=1; ii<iFilenoReq; ii++)
      {
         pQueryObj++;
      }

      iRC = rawQueryString(pQueryObj, iFull, pInfo);
      if (iDebug) printf(
         "-D- requested file no %d found:\n    %s", iFilenoReq, pInfo);
      goto gDone;
   }

   /******************** get ADSM representation ********************/

   strcpy(pcNamefs, pFile);
   pcc = (char *) strchr(pcNamefs, *pcObjDelim);
   if (pcc == NULL)
   {
      printf("-E- invalid remote file name '%s'\n", pFile);
      return -1;
   }

   if (strncmp(pcc, pFile, 1) == 0)      /* skip leading delimiter */
      pcc++;
   pcc = (char *) strchr(pcc, *pcObjDelim);
   if (pcc == NULL)
   {
      printf("-E- invalid remote file name '%s'\n", pFile);
      return -1;
   }
   strcpy(pcNamehl, pcc);     /* starts with high level object name */
   strncpy(pcc, "\0", 1);                     /* terminates FS name */

   pcc = pcNamehl;
   pcc++;                                 /* skip leading delimiter */
   pcc = (char *) strrchr(pcc, *pcObjDelim);
   if (pcc == NULL)
   {
      printf("-E- invalid remote file name '%s'\n", pFile);
      return -1;
   }
   strcpy(pcNamell, pcc);      /* starts with low level object name */
   strncpy(pcc, "\0", 1);      /* terminates high level object name */

   if (iDebug)
      printf("    FS name '%s', HL name '%s', LL name '%s'\n",
             pcNamefs, pcNamehl, pcNamell);

   strcpy(pComm->cNamefs, (char *) rawGetFSName(pcNamefs));
   if (strlen(pComm->cNamefs) == 0)
      return -1;

   strcpy(pComm->cNamehl, (char *) rawGetHLName(pcNamehl));
   /* strcpy(pComm->cNamell, (char *) rawGetLLName(pcNamell)); */
   iRC = rawGetLLName(pcNamell, pcObjDelim, pComm->cNamell);

   if (iDebug)
      printf("    FS name '%s', HL name '%s', LL name '%s'\n",
             pComm->cNamefs, pComm->cNamehl, pComm->cNamell);

   /***************** get client info, contact server ***************/

   strcpy(cOwner, (char *) rawGetUserid());
   iRC = gethostname(cliNode, MAX_NODE);
   if (iRC)
   {
      printf("-E- getting client host name: %s\n", strerror(iRC));
      return -1;
   }

   if (iDebug)
      printf("    user %s on node %s, platform %s\n",
             cOwner, cliNode, cOS);            /* cOS from rawcli.h */

   strcpy(pComm->cApplType, cApplType);
   strcpy(pComm->cOwner,  cOwner);
   strcpy(pComm->cliNode, cliNode);
   strcpy(pComm->cOS, cOS);

   pcc = (char *) pComm;
   if ( (iRC = send( iSocket, pcc, iCommSize, 0 )) < 0 )
   {
      printf("-E- %s: query file %s\n", cModule, pComm->cNamell);
      perror("    sending command buffer");
      return -1;
   }

   if (iDebug) printf(
      "-D- query command sent to server (%d bytes), look for reply\n",
      iCommSize);

   /******************** look for reply from server *****************/

   while (!bQDone)                       /* loop over query buffers */
   {
      pcc = (char *) pQueryResult;
      iRC = rawRecvHead(iSocket, &pcc);
      if (iRC <= 0)
      {
         printf("-E- receiving buffer header with query information\n");
         return(-1);
      }
      else if (iDebug > 0) printf("    %d bytes received\n", iRC);

      pcc += HEAD_LEN;
      iIdent = ntohl(pQueryResult->iIdent);
      iquery = ntohl(pQueryResult->iObjCount);
      iAttrLen = ntohl(pQueryResult->iAttrLen);

      if (iIdent != IDENT_QUERY)
      {
         if (iIdent == IDENT_STATUS)
         {
            iStatus = iquery;
            iStatusLen = iAttrLen;
            if ( (iStatus == STA_ERROR) || (iStatus == STA_ERROR_EOF) )
            {
               if (iDebug) printf("\n");
               printf("-E- received error status from server");
               if (iDebug)
                  printf(" instead of query data:\n");
               else printf(":\n");

               if (iStatusLen > 0)
               {
                  iRC = rawRecvError(iSocket, iStatusLen, &pcc);
                  if (iRC < 0) printf(
                     "-E- receiving error msg from server, rc = %d\n",
                     iRC);
                  else printf("    %s\n", pcc);
               }
               else printf("    no error message available\n");

            } /* iStatus == STA_ERROR || (iStatus == STA_ERROR_EOF) */
            else printf(
              "-E- unexpected status (type %d) received from server\n",
              iStatus);

         } /* iIdent == IDENT_STATUS */
         else printf(
            "-E- unexpected header (type %d) received from server\n",
            iIdent);

         return -1;

      } /* iIdent != IDENT_QUERY */

      if (iquery == 0)
      {
         bQDone = bTrue;
         if (iQueryCount == 0)
            printf("-I- no matching objects found\n");
         else
            if (iDebug) printf("    no more objects\n");

      } /* (iquery == 0) */
      else
      {
         iBuf = iquery*iAttrLen;
         iBufComm = iBuf;
         while(iBuf > 0)
         {
            if ( (iRC = recv( iSocket, pcc, iBuf, 0 )) < 0 )
            {
               perror("-E- receiving query results");
               return -1;
            }
            iBuf -= iRC;
            pcc += iRC;
         } /* while(iBuf > 0) */

         iqueryBuf++;

         if (iDebug) printf(
            "*** query buffer %d (%d bytes): %d objects\n",
                            iqueryBuf, iBufComm, iquery);

         if (iquery < iqueryMax)
         {
            bQDone = bTrue;
            if (iDebug)
               printf("    query finished\n");
         }
         else if (iDebug)
            printf("    possibly more query buffers outstanding\n");

         /************** handle buffer with query results ************/

         /* loop over objects of current query buffer */
         pQueryObj = pQueryObj0; /* set ptr to first object in buffer */
         for (ii=1; ii<=iquery; ii++)
         {
            iQueryCount++;                  /* no. of current object */
            if (iDebug)
            {
               iRC = rawQueryString(pQueryObj, iFull, pInfo);
               printf("%d: %s", iQueryCount, pInfo);
            }
            pQueryObj++;                /* points now to next object */

         } /* loop over objects of current query buffer */
      } /* (iquery > 0) */

   } /* while (!bQDone) */

gDone:
   if (iDebug)
   {
      if (iFilenoReq == 0)
         printf("-D- %d matching objects %s%s%s found\n",
                iQueryCount,
                pComm->cNamefs,
                pComm->cNamehl,
                pComm->cNamell);

      printf("-D- end %s\n\n", cModule);
   }

   if ( (iQueryCount) && (iFilenoReq == 0) )
      iRC = rawQueryString(pQueryObj0, iFull, pInfo);

   return iQueryCount;

} /* end rawQueryList */

/**********************************************************************
 * rawScanObjbuf:
 *   check if obj already available in obj buffer chain
 * created 10. 7.2003 by Horst Goeringer
 **********************************************************************
 */

int rawScanObjbuf(char *pcFile, int iObjnoMax, int *piObjBuf0)
{
   int iDebug = 0;
   char cModule[32] = "rawScanObjbuf";
   int ii, jj = 0;
   int iFound = 0;   /* =1: specified file already available in list */
   int iObjnoBuf;                /* no. of objects in current buffer */
   int iObjnoAll = 0;               /* sum of objects in all buffers */
   int iBufno = 0;                          /* no. of object buffers */
   int *piObjBuf;                    /* ptr to current object buffer */
   int **ppiNextBuf;

   srawRetrList *psRetrList;     /* points to object in buffer chain */

   if (iDebug)
      printf("\n-D- begin %s: check for %s\n", cModule, pcFile);

   piObjBuf = piObjBuf0;
   /* loop over object buffers */
   for (;;)
   {
      iObjnoBuf = *piObjBuf;
      psRetrList = (srawRetrList *) &(piObjBuf[1]);
      iBufno++;

      /* loop over objects in current buffer */
      for (ii=1; ii<=iObjnoBuf; ii++)
      {
         jj++;
         if (iDebug == 2)
            printf("    %d: %s\n", jj, psRetrList->cNamell);

         if (strcmp(pcFile, psRetrList->cNamell) == 0)
         {
            if (iDebug) printf(
               "    file %s already available in list\n", pcFile);
            iFound = 1;
            goto gEndScan;
         }

         psRetrList++;

      } /* loop over objects in current buffer */

      if (iObjnoBuf < iObjnoMax)                      /* last buffer */
         break;

      if (psRetrList)
      {
         ppiNextBuf = (int **) psRetrList;
         piObjBuf = *ppiNextBuf;
         psRetrList = (srawRetrList *) &(piObjBuf[1]);
      }
      else                                            /* last buffer */
         break;

   } /* loop over buffers */

gEndScan:
   if (iDebug)
      printf("-D- end %s: %d buffers scanned\n\n", cModule, iBufno);

   if (iFound)
      return 1;
   else
      return 0;

} /* end rawScanObjbuf */

#ifdef _AIX
/*********************************************************************
 * ielpst: Elapsed (wall-clock) time measurement
 *
 *    Usage examples:
 *
 *  ielpst( 1, iBuf[0..1] );
 *  ... code ...
 *  ElapsedTime(in microsecs) = ielpst( 1, iBuf[0..1] );
 *
 *  ielpst( 1000, iBuf[0..1] );
 *  ... code ...
 *  ElapsedTime(in millisecs) = ielpst( 1000, iBuf[0..1] );
 *
 *********************************************************************
 * created 17.11.1992, Michael Kraemer
 *********************************************************************
 */

unsigned long ielpst( unsigned long iScale,
                      unsigned long *iBuf )
{
   int iDebug = 0;
   char cModule[32] = "ielpst";
   struct timestruc_t stv;
   unsigned long iTemp[2];

   gettimer( TIMEOFDAY, &stv );

   if ( iScale <= 0 ) iScale = 1;
   iTemp[1] = stv.tv_sec  - iBuf[1];
   if ( ( iTemp[0] = stv.tv_nsec - iBuf[0] ) & 0x80000000 ) {
      iTemp[0] += 1000000000L; iTemp[1] -= 1;
   }
   iBuf[1] = stv.tv_sec; iBuf[0] = stv.tv_nsec;
   return( iTemp[1] * ( 1000000L / iScale ) + ( iTemp[0] / 1000 ) / iScale );

} /* ielpst */

/* FORTRAN entry */
unsigned long ielpst_( unsigned long *iScale,
                       unsigned long *iBuf )
{
   return( ielpst( *iScale, iBuf ) );
} /* ielpst_ */
#endif

