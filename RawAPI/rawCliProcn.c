/**********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 28. 8.1996 by Horst Goeringer
 **********************************************************************
 * rawCliProcn.c:
 *    utility programs for gStore package: clients only
 **********************************************************************
 * rawCheckFilelist: remove objects already archived from file list
 * rawCheckObjlist:  remove existing files from object and file list,
 *                   reorder according to restore parameter
 * rawDelFile:       delete single file in GSI mass storage
 * rawDelList:       delete list of files in GSI mass storage
 * rawGetFilelistEntries: get filelist entries from input file 
 * rawGetWSInfo:     get workspace and pool info from master
 * rawGetFullFile:   get full file name from generic input & ll name
 * rawQueryPrint:    print query results for one object to stdout
 *                   due to performance separate from rawQueryString
 * rawQueryString:   print query results for one object to string
 * rawScanObjbuf:    check if obj already available in obj buffer chain
 * rawSortValues:    sort indexed list of numbers
 * ielpst:           AIX only: elapsed (wall-clock) time measurement
 **********************************************************************
 * 26. 3.1996, H.G.: rawCheckObjlist: work on ptr-linked object buffers
 * 21.11.1997, H.G.: rawDelFile: only warning (rc=1) if obj not found
 * 27. 7.1999, H.G.: rawGetFullFile: don't check matches, rely on ADSM
 * 19.12.2000, H.G.: rawCheckObjlist: remove unused code, better doc
 * 18. 5.2001, H.G.: merge into new file rawcliproc.c:
 *                   rawDelFile.c
 *                   rawDelList (rawDelFilelist from rawproc.c)
 *                   rawTapeFile.c
 *                   ielpst.c
 * 20. 7.2001, H.G.: rawCheckObjlist: mover node in srawRetrList
 * 24. 8.2001, H.G.: rawCheckObjlist: stage status in srawRetrList
 * 24. 1.2002, H.G.: created from rawcliproc.c, ACTION DELETE -> REMOVE
 * 13. 2.2002, H.G.: rawDelFile: handle also stage file
 * 25. 4.2002, H.G.: move rawTapeFile to new file rawProcTape.c
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
 *  5. 3.2004, H.G.: rawCheckObjlist: add cArchiveDate, cOwner to
 *                   srawRetrList
 *  6. 8.2004, H.G.: ported to Lynx
 * 26. 1.2005, H.G.: rawapitd-gsi.h -> rawapitd-gsin.h
 * 22. 4.2005, H.G.: rawScanObjbuf: handle filelist of any length
 *  3. 4.2006, H.G.: ported to sarge
 *  9. 5.2006, H.G.: rename QUERY_RETRIEVE_API -> QUERY_RETRIEVE_RECORD
 *  7.12.2006, H.G.: rawDelList: add argument iSocketMaster
 * 26.10.2007, H.G.: rawGetWSInfo: readable pool space information
 * 19. 3.2008, H.G.: rawCheckObjlist: use extended stage/cache parameters
 * 14. 4.2008, H.G.: rawGetFilelistEntries: allow fully qualified filelist
 * 21. 4.2008, H.G.: rawGetWSInfo: show only free HW, not free pool space
 *  8. 5.2008, H.G.: rawGetFullFile: moved from rawProcn.c
 *                   rawQueryFile: moved -> rawProcn.c
 *                   rawQueryList: moved -> rawProcn.c
 *  9. 5.2008, H.G.: replace srawArchList (in rawclin.h, removed)
 *                   by srawFileList (added to rawcommn.h)
 * 11. 6.2008, H.G.: rawGetFilelistEntries: correct handling of case
 *                   only invalid entries in filelist
 * 16. 6.2008, H.G.: rawCheckObjlist: take all 5 restore fields into
 *                   account for reordering
 * 17. 6.2008, H.G.: rawGetFilelistEntries: use cDataFSHigh
 * 16. 9.2008, H.G.: rawGetFilelistEntries: cDataFSHigh->cDataFSHigh1,2
 *  6.11.2008, H.G.: rawGetWSInfo: improved handling rc=0 from recv()
 * 11.11.2008, H.G.: add harmless suggestions of Hakan
 *  3.12.2008, H.G.: add suggestions of Hakan, part II
 *  9. 6.2009, H.G.: replace perror by strerror
 * 10. 6.2009, H.G.: rawCheckObjlist: new sort criteria:
 *                   file in RC or WC: first and second in sort order
 *                   all 5 restore order parameters (3rd - 7th)
 *                   scan for all sort criteria over all query buffers
 * 15. 6.2009, H.G.: rawSortValues: sort indexed list of numbers
 * 14. 7.2009, H.G.: rawCheckObjlist: when copying tape file entries:
 *                   check for new input buffer also if file in RC/WC
 * 27.11.2009, H.G.: rawCheckObjlist: handle enhanced srawRetrList
 * 30.11.2009, H.G.: rawCheckFilelist: new arg pcArchive
 * 10.12.2009, H.G.: rawDelFile: handle enhanced structure srawObjAttr
 *                   old version info also available in new structure
 * 29. 1.2010, H.G.: replace MAX_FILE -> MAX_FULL_FILE in:
 *                      static string cPath 
 *                      rawGetFilelistEntries:
 *                         cPath, cEntry, cTemp, cFileName
 *                      rawGetWSInfo: pcGenFile
 *                      rawGetFullFile: cname1, cPath
 *  3. 2.2010, H.G.: make 'archive -over' working again
 *                   rawDelFile: provide correct objId in command buffer
 *                   rawDelList: fix bug setting local ptr to ll name
 *  4. 2.2010, H.G.: rawDelList: provide all infos for rawDelFile
 *                   rawDelFile: no more query for obj needed  
 *  5. 2.2010, H.G.: move rawQueryPrint,rawQueryString from rawProcn.c
 * 10. 2.2010, H.G.: rawQueryPrint, rawQueryString: remove 
 *                      filetype, buffersize from output
 * 26. 2.2010, H.G.: rawQueryString: add parameter (len output string)
 *                      handle too short output string
 * 19. 3.2010, H.G.: rawGetFilelistEntries: add parameter (dir in list)
 * 12. 5.2010, H.G.: rawScanObjbuf: additional arg pathname,
 *                      for recursive file handling
 * 19. 5.2010, H.G.: rawCheckObjlist: streamline filelist handling
 * 18. 8.2010, H.G.: rawGetFilelistEntries: stricter limit check,
 *                      remove trailing '/' in global path, if specified
 *  6. 9.2010, H.G.: replace print formats for 'int': %lu -> %u
 * 24. 9.2010, H.G.: rawQueryPrint, rawQueryString: 64 bit filesizes
 * 26.11.2010, H.G.: use string cTooBig for filesizes>=4GB
 **********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef Lynx
#include <socket.h>
#include <time.h>
#else
#include <sys/socket.h>
#include <sys/time.h>
#endif

#ifdef Linux
#include <netinet/in.h>
#endif

#include "rawapitd.h"
#include "rawapitd-gsin.h"
#include "rawcommn.h"
#include "rawdefn.h"
#include "rawclin.h"
#include "rawentn.h"
#include "rawapplcli.h"

extern FILE *fLogFile;
extern int *piEntryList;                /* for rawGetFilelistEntries */

static unsigned int iint = sizeof(int);
static unsigned int iRetrList = sizeof(srawRetrList);
static unsigned int iFileList = sizeof(srawFileList);

static char cPath[MAX_FULL_FILE] = "";
                  /* returned in rawGetFullFile, must not be locally */

/*********************************************************************
 * rawCheckFilelist: remove objects already archived from file list
 *    returns number of removed objects
 *
 * created 15. 3.96, Horst Goeringer
 * 25. 3.1996, H.G.: work on ptr-linked object buffers
 *********************************************************************
 */

int rawCheckFilelist(char **pcFileList, char **pcObjList,
                     char *pcArchive, char *pcNodeMaster)
{
   char cModule[32] = "rawCheckFilelist";
   int iDebug = 0;

   char cDelimiter[1] = "";
   char *pcfl, *pcol;
   int *pifl, *piol;
   int ifile0, ifile;
   int iobj0, iobj;

   srawFileList *psFile, *psFile0;        /* files to be archived */
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
   psFile = (srawFileList *) ++pifl;  /* points now to first file */
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
      fprintf(fLogFile, "    first file %s, first obj %s%s%s\n",
         psFile->cFile, pcArchive, psObj->cNamehl, psObj->cNamell);
   }

   iRC = strncmp(psObj->cNamell, pcObjDelim, 1);
   if (iRC)
   {
      iRC = strncmp(psObj->cNamell, pcObjDelimAlt, 1);
      if (iRC)
      {
         fprintf(fLogFile,
            "-E- %s: invalid object delimiter %1s found\n",
            cModule, psObj->cNamell);
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
      iRC = rawGetLLName(psFile->cFile, cDelimiter, cFilell);

      if (iDebug)
         fprintf(fLogFile, "    %d: '%s'\n", ii, cFilell);

      bLastBuf = bFalse;
      iobjBuf = 1;
      if (iDebug > 1)
         fprintf(fLogFile, "\n*** buffer %d: %d objs\n", iobjBuf, iobj);

      while (!bLastBuf)
      {

         for (jj = 1; jj <= iobj; jj++)         /* object loop */
         {

            iRC = strcmp(cFilell, psObj->cNamell);
            if (iRC == 0)                      /* file archived */
            {
              if (iDebug) fprintf(fLogFile,
                 "       %s%s%s already archived in gStore\n", 
                 pcArchive, psObj->cNamehl, pcFilell);
              else fprintf(fLogFile,
                 "-W- %s%s%s already archived in gStore\n", 
                 pcArchive, psObj->cNamehl, pcFilell);

               strncpy(psFile->cFile, "\n", 1);
               ifile--;
               if (ifile == 0) 
               {
                  if (iDebug) fprintf(fLogFile,
                      "    last file, comparison finished\n");
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
                      ii, psFile->cFile, cFilell);
         }
         else
         {
            iobjBuf++;
            pinext = *piptr;
            iobj = *pinext++;
            psObj = (srawRetrList *) pinext;
            if (iDebug > 1) fprintf(fLogFile,
               "\n*** new buffer %d: %d objs, first obj '%s'\n", 
               iobjBuf, iobj, psObj->cNamell);
         }

      } /* while (!bLastBuf) */

gNextFile:
      psFile++;
   } /* file loop */

gEndCompare:
   idel = ifile0-ifile;
   if (iDebug) fprintf(fLogFile,
      "    %d of %d files removed: %d files remaining\n",
      idel, ifile0, ifile);

   pifl[0] = ifile;
   *pcFileList = (char *) pifl;

   if ( (ifile > 0) && (idel > 0) )        /* compress filelist */
   {
      iif = 1;
      psFile = psFile0;
      for (ii = 1; ii <= ifile0; ii++)             /* file loop */
      {
         /* fprintf(fLogFile, "%d: %s\n", ii, psFile->cFile); */
         iRC = strncmp(psFile->cFile, "\n", 1);
         if (iRC != 0)                            /* file found */
         { 
            if (iif >= ifile0)
               break;

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
               iRC = strncmp(psFile->cFile, "\n", 1);
               if (iRC == 0) 
               {
                  if (iif >= ifile0)
                     break;
               }
               else
               {
                  strcpy(psFile0->cFile, psFile->cFile);
                  if (iDebug > 1) fprintf(fLogFile,
                     "*** %d: %s\n", iif, psFile0->cFile); 
                  if (iif >= ifile0)
                     break;

                  psFile0++;
               }
            } /* for (jj...) */

         } /* compress action */ 
      } /* for (ii...) */

      if (iDebug)
         fprintf(fLogFile, "    file list compressed\n");

   } /* ifile > 0 && idel > 0 */


   if (iDebug)
   {
      fprintf(fLogFile, "-D- end %s\n\n", cModule);
      fflush(fLogFile);
   }

   return idel ;

} /* rawCheckFilelist */

/********************************************************************
 * rawCheckObjlist:
 *    iFileComp: no. of valid files/objs
 *    on input:
 *       pcObjList: chain of ptr-linked buffers with obj lists
 *       pcFileList: corresponding file list (one buffer)
 *       in both invalid elements (retrieve: already existing) with
 *       file names eliminated to "\0"
 *    on output (one buffer):
 *       pcObjComp:  corresponding compressed and sorted obj list
 *       pcFileList: compressed and sorted file list
 *    internal (one buffer):
 *       pcObjSort:  compressed intermediate results
 *       pcFileSort: compressed intermediate results
 *       pcFileComp: compressed and sorted file list, to be copied to
 *                   pcFileList
 *
 *    compress pcObjList -> pcObjComp
 *    (retrieve, stage, and unstage)
 *
 *    remove files/objs with 0 length ll names from obj + file lists
 *    (retrieve only)
 *
 *    reorder remaining files/objs according to 
 *       (1) read cache
 *       (2) write cache
 *       (3) 5 restore parameters
 *    (retrieve and stage only)
 *
 * created 18. 3.96, Horst Goeringer
 ********************************************************************
 */

int rawCheckObjlist(
       int iFileComp, 
       int iSort,         /* =0: only compress of obj list (unstage) */
       char **pcObjList,       /* chained buffer list with obj names */
       char **pcFileList,                  /* buffer with file names */
       char **pcObjComp)  /* compressed output buffer with obj names */
{
   char cModule[32]="rawCheckObjlist";
   int iDebug = 0;

   int iRC;
   int iObjComp;
   int iDiff = 0;
   char cMsg[STATUS_LEN] = "";

   /* int iaValue[iFileComp][6]; cannot be used:
      writing into array overwrites other program variables !!!!!!!! */

   int iaValue[MAX_FILE_NO][6];
      /* for sorting relevant values of all objs
       * 1st index: file/object no.
       * 2nd index 0: = 0: ignore (file 
       *              = 1: file in read cache 
       *              = 2: file in write cache 
       *              = 3: sort according to up to 5 TSM restore params
       * 2nd index 1 - 5: value of corresponing TSM restore parameters
       */
   int iaCompValue[MAX_FILE_NO][6];
      /* compressed list of for sorting relevant values */
   int iaIndex[MAX_FILE_NO];
                   /* sort index for each obj used after compression */

   int iaIndNew[MAX_FILE_NO];
   int iaValTop[MAX_FILE_NO];
   int iaValHiHi[MAX_FILE_NO];
   int iaValHiLo[MAX_FILE_NO];
   int iaValLoHi[MAX_FILE_NO];
   int iaValLoLo[MAX_FILE_NO];

   int iif = 0, ii1 = 0, ii2 = 0;
   int iisort1 = 0,                /* no. of 1st object to be sorted */
       iisort2 = 0;               /* no. of last object to be sorted */
   int iitop1 = 0,                  /* no. of 1st topId to be sorted */
       iitop2 = 0,                 /* no. of last topId to be sorted */
       iTopValcur = 0,                   /* no. of current top value */
       iObjTopValcur = 0;/* cur no. of object with current top value */
   int iihihi1 = 0, iihihi2 = 0, iHiHiValcur = 0, iihihicur = 0;
   int iihilo1 = 0, iihilo2 = 0, iHiLoValcur = 0, iihilocur = 0;
   int iilohi1 = 0, iilohi2 = 0, iLoHiValcur = 0, iilohicur = 0;

   int iFileSort = 0,              /* no. of TSM files: to be sorted */
       iReadCache = 0,                 /* no. of files in read cache */
       iWriteCache = 0;               /* no. of files in write cache */

   /* no. of files using restore field */
   int iRestoTop = 0,
       iRestoHiHi = 0,
       iRestoHiLo = 0,
       iRestoLoHi = 0,
       iRestoLoLo = 0;

   /* no. of different restore fields */
   unsigned int iRestoTopValues = 0,
                iRestoHiHiValues = 0,
                iRestoHiLoValues = 0,
                iRestoLoHiValues = 0,
                iRestoLoLoValues = 0;

   unsigned int iRestoTopMin = 0,
                iRestoHiHiMin = 0,
                iRestoHiLoMin = 0,
                iRestoLoHiMin = 0,
                iRestoLoLoMin = 0;

   bool_t bRestoSort = bFalse;
                         /* if true: subset of restore fields sorted */
   bool_t bRestoTop = bFalse,     /* if true: top restore field used */
          bRestoHiHi = bFalse, /* if true: HighHigh resto field used */
          bRestoHiLo = bFalse,/* if true: HighLow restore field used */
          bRestoLoHi = bFalse,/* if true: LowHigh restore field used */
          bRestoLoLo = bFalse; /* if true: LowLow restore field used */

   char *pcfl, *pcol;
   int  *pifl, *piol;
   int iFileAll = 0;  /* overall number of files (buffer pcFileList) */
   int iobj0 = 0,   /* number of objects in current buffer pcObjList */
       iobj = 0;

   int iFileBufComp,      /* size of buffer with compressed filelist */ 
       iObjBufComp;        /* size of buffer with compressed objlist */
   /*    pcObjComp in arg list */
   char *pcFileComp;         /* local file buffer: filled in sorting */
   char *pcflcomp, *pcolcomp;
   int  *piflcomp, *piolcomp;

   char *pcFileSort,         /* local file buffer: filled in sorting */
        *pcObjSort;        /* local object buffer: filled in sorting */
   char *pcflsort, *pcolsort;
   int  *piflsort, *piolsort;

   int iiObj = 0;      /* no. of file/object found in current buffer */
   int iiBuf = 0;                    /* no. of current object buffer */
   int iiFile = 0;      /* no. of file/object used in current buffer */

   int **piptr;           /* points to pointer to next object buffer */
   int *pinext;                      /* points to next object buffer */

   srawFileList *psFile, *psFile0;             /* files in file list */
   srawFileList *psFileSort,             /* files in local file list */
                *psFileSort0,         /* 1st file in local file list */
                *psFileComp,             /* files in local file list */
                *psFileComp0;         /* 1st file in local file list */

   srawRetrList *psObj;          /* current object in current buffer */
   srawRetrList *psObj0;               /* 1st object (in 1st buffer) */
   srawRetrList *psObjSort,          /* objects in local object list */
                *psObjSort0,      /* 1st object in local object list */
                *psObjComp,         /* objects in output object list */
                *psObjComp0;     /* 1st object in output object list */

   int ii, jj, jjMin = 0;
   int icount = 0;

   bool_t bInit;

   /* file list (input and output) */
   pcfl = *pcFileList;
   pifl = (int *) pcfl;
   iFileAll = pifl[0];                        /* number of all files */
   psFile = (srawFileList *) ++pifl;     /* points now to first file */
   psFile0 = psFile;
   pifl--;                        /* points again to number of files */

   /* linked object list (input) */
   pcol = *pcObjList;
   piol = (int *) pcol;
   iobj0 = piol[0];          /* number of archived objects in buffer */
   iobj = iobj0;
   psObj = (srawRetrList *) ++piol;    /* points now to first object */
   psObj0 = psObj;
   piol--;                      /* points again to number of objects */

   /* object list (output) */
   pcolcomp = *pcObjComp;
   piolcomp = (int *) pcolcomp;
   psObjComp = (srawRetrList *) ++piolcomp;          /* first object */
   psObjComp0 = psObjComp;
   piolcomp--;

   if (iFileComp >= MAX_FILE_NO)
   {
      ii = MAX_FILE_NO-1;
      fprintf(fLogFile,
         "-E- %s: file list too long: %d entries, only %d allowed\n",
         cModule, iFileComp, ii);
      return -2;
   }

   if (iDebug)
   {
      fprintf(fLogFile,
         "\n-D- begin %s: overall %d objects, in 1st buffer %d objects\n",
         cModule, iFileAll, iobj0);
      fprintf(fLogFile, "    compressed: %d objects\n", iFileComp);
      if (iDebug > 1)
      {
         if (iSort == 1) fprintf(fLogFile,
            "    first file %s, first obj %s%s\n",
            psFile->cFile, psObj->cNamehl, psObj->cNamell);
         else fprintf(fLogFile,
            "    first obj %s%s\n", psObj->cNamehl, psObj->cNamell);
      }
   }

   if (iDebug == 2)
   {
      for (jj=1; jj<=iFileAll; jj++)
      {
         fprintf(fLogFile,
            "DDDD %d: file %s (%d)\n", jj, psFile->cFile, psFile);
         psFile++;
      }
      psFile = psFile0; /* reset */
   }

   /************ unstage: only compress object buffer ****************/

   if (iSort == 0)
   {
      iiBuf = 1;                               /* running buffer no. */
      ii = 0;

      /* loop over all objects */
      for (jj=1; jj<=iFileAll; jj++)
      {
         iiObj++;
         if (strlen(psObj->cNamell) < 2)
         {
            if (iDebug == 2) fprintf(fLogFile,
               "    object %d(%d): ignored\n", iiObj, jj);

            goto gNextObjUnstage;
         }

         ii++;
         memcpy((char *) psObjComp, (char *) psObj, iRetrList);
         if (iDebug) fprintf(fLogFile,
            "%d(%d): obj %s%s copied (Comp)\n", iiObj, jj,
            psObjComp->cNamehl, psObjComp->cNamell);
            
         psObjComp++;

gNextObjUnstage:
         psObj++;
         if (iiObj == iobj)
         {
            if (iDebug) fprintf(fLogFile,
               "      buffer %d: last obj (no. %d) handled\n",
               iiBuf, iiObj);

            piptr = (int **) psObj;
            if (*piptr == NULL)
            {
               if (iDebug) fprintf(fLogFile,
                  "      buffer %d: last obj buffer\n", iiBuf);
            }
            else
            {
               iiObj = 0;
               iiBuf++;
               pinext = *piptr;
               iobj = *pinext++;
               psObj = (srawRetrList *) pinext;
               if (iDebug) fprintf(fLogFile,
                  "      new buffer %d, %d objs, first: %s%s|\n",
                  iiBuf, iobj, psObj->cNamehl, psObj->cNamell);
            }
         } /* iiObj == iobj */
      } /* loop over all objects */

      *piolcomp = ii;
      goto gEndCheckObjlist;

   } /* (iSort == 0) */

   /************* allocate local file/object buffers *****************/

   iFileBufComp = sizeof(int) + iFileComp*sizeof(srawFileList);
   if ((pcFileComp = (char *) calloc(
           (unsigned) iFileBufComp, sizeof(char) ) ) == NULL)
   {
      fprintf(fLogFile,
         "-E- %s: allocating filelist buffer (Comp, %d byte)\n",
         cModule, iFileBufComp);
      if (errno)
         fprintf(fLogFile, "    %s\n", strerror(errno));
      perror("-E- allocating filelist buffer (Comp)");

      return -2;
   }
   if (iDebug) fprintf(fLogFile,
      "    filelist buffer (Comp) allocated (size %d)\n", iFileBufComp);

   piflcomp = (int *) pcFileComp;
   piflcomp[0] = iFileComp;            /* compressed number of files */
   psFileComp = (srawFileList *) ++piflcomp;           /* first file */
   psFileComp0 = psFileComp;

   if ( (pcFileSort = (char *) calloc(
            (unsigned) iFileBufComp, sizeof(char)) ) == NULL)
   {
      fprintf(fLogFile,
         "-E- %s: allocating filelist buffer (Sort, %d byte)\n",
         cModule, iFileBufComp);
      if (errno)
         fprintf(fLogFile, "    %s\n", strerror(errno));
      perror("-E- allocating filelist buffer (Sort)");

      return(-2);
   }
   if (iDebug) fprintf(fLogFile,
      "    filelist buffer (Sort) allocated (size %d)\n", iFileBufComp);

   piflsort = (int *) pcFileSort;
   piflsort[0] = iFileComp;            /* compressed number of files */
   psFileSort = (srawFileList *) ++piflsort;           /* first file */
   psFileSort0 = psFileSort;

   iObjBufComp = sizeof(int) + iFileComp*sizeof(srawRetrList);
   if ( (pcObjSort = (char *) calloc(
            (unsigned) iObjBufComp, sizeof(char)) ) == NULL )
   {
      fprintf(fLogFile,
         "-E- %s: allocating objectlist buffer (%d byte)\n",
         cModule, iObjBufComp);
      if (errno)
         fprintf(fLogFile, "    %s\n", strerror(errno));
      perror("-E- allocating objlist buffer");

      return(-3);
   }
   if (iDebug) fprintf(fLogFile,
      "    objlist buffer (Sort) allocated (size %d)\n", iObjBufComp);

   pcolsort = pcObjSort;
   piolsort = (int *) pcolsort;
   piolsort[0] = iFileComp;          /* compressed number of objects */
   psObjSort = (srawRetrList *) ++piolsort;          /* first object */
   psObjSort0 = psObjSort;
 
   /************** scan ALL objects to get sort criteria *************/

   for (jj=1; jj<=iFileAll; jj++)
   {
      iaIndex[jj] = -1;                                      /* init */
      for (ii=0; ii<=5; ii++)
      {
         if (ii == 0)
            iaValue[jj][ii] = -1;                            /* init */
         else
            iaValue[jj][ii] = 0;                             /* init */
      }
   }

   psObj = psObj0;                       /* 1st object in 1st buffer */
   iobj = iobj0;                     /* no. of objects in 1st buffer */
   iiObj = 0;                        /* running object no. in buffer */
   iiBuf = 1;                                  /* running buffer no. */

   if (iDebug)
      fprintf(fLogFile, "scan objects for sort criteria:\n");

   /* loop over ALL objects to get sort criteria */
   for (jj=1; jj<=iFileAll; jj++)
   {
      iiObj++;                      /* counter all objects in buffer */

      if (iDebug == 2) fprintf(fLogFile,
         "DDD %d (comp %d): obj %s (%d), file %s (%d)\n",
         iiObj, jj, psObj->cNamell, psObj, psFile->cFile, psFile);

      if (strlen(psObj->cNamell) < 2)
      {
         iaValue[jj][0] = 0;                               /* ignore */
         if (iDebug == 2) fprintf(fLogFile,
            "    object %d(%d): ignored\n", iiObj, jj);

         goto gNextValue;
      }

      if (psObj->iStageFS)
      {
         if (iDebug == 2) fprintf(fLogFile,
            "    object %d(%d) %s%s: in read cache\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell);

         iReadCache++;
         iaValue[jj][0] = 1;

         /* copy to final destination */
         memcpy((char *) psObjComp, (char *) psObj, iRetrList);
         memcpy(psFileComp->cFile, psFile->cFile, iFileList);

         if (iDebug)
         {
            fprintf(fLogFile,
               "%d: read cache obj %s%s (objId %u-%u) copied (Comp)\n",
               iReadCache, psObjComp->cNamehl, psObjComp->cNamell,
               psObjComp->iObjHigh, psObjComp->iObjLow);
            fprintf(fLogFile,
               "    orig file name %s (%d)\n",
               psFile->cFile, psFile);
            fprintf(fLogFile,
               "    file name %s copied (Comp: %d)\n",
               psFileComp->cFile, psFileComp);
            if (jj == iFileAll) fprintf(fLogFile,
               "       in last buffer %d: last obj (no. %d) handled\n",
               iiBuf, iiObj);
         }

         /* mark as already handled
         strncpy(psObj->cNamell, "\0", 1); */

         psObjComp++;
         psFileComp++;
         goto gNextValue;
      }

      if (psObj->iCacheFS)
      {
         if (iDebug == 2) fprintf(fLogFile,
            "    object %d(%d) %s%s: in write cache\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell);

         iWriteCache++;
         iaValue[jj][0] = 2;

         /* copy to temp. destination */
         memcpy((char *) psObjSort, (char *) psObj, iRetrList);
         memcpy(psFileSort->cFile, psFile->cFile, iFileList);
         if (iDebug)
         {
            fprintf(fLogFile,
               "%d: write cache obj %s%s (%u-%u) copied (Sort)\n",
               iWriteCache, psObjSort->cNamehl, psObjSort->cNamell,
               psObjSort->iObjHigh, psObjSort->iObjLow);
            
            if (jj == iFileAll) fprintf(fLogFile,
               "       in last buffer %d: last obj (no. %d) handled\n",
               iiBuf, iiObj);
         }

         psObjSort++;
         psFileSort++;
         goto gNextValue;
      }

      iaValue[jj][0] = 3;
      if (psObj->iRestoHigh)
      {
         iaValue[jj][1] = psObj->iRestoHigh;
         iRestoTop++;
         if (iDebug == 2) fprintf(fLogFile,
            "    object %d(%d) %s%s: top restore field %d\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell, iaValue[jj][1]);
      }
      if (psObj->iRestoHighHigh)
      {
         iaValue[jj][2] = psObj->iRestoHighHigh;
         iRestoHiHi++;
         fprintf(fLogFile,
            "-W- object %d(%d) %s%s: hihi restore field %d\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell, iaValue[jj][2]);
      }
      if (psObj->iRestoHighLow)
      {
         iaValue[jj][3] = psObj->iRestoHighLow;
         iRestoHiLo++;
         if (iDebug == 2) fprintf(fLogFile,
            "    object %d(%d) %s%s: hilo restore field %d\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell, iaValue[jj][3]);
      }
      if (psObj->iRestoLowHigh)
      {
         iaValue[jj][4] = psObj->iRestoLowHigh;
         iRestoLoHi++;
         fprintf(fLogFile,
            "-W- object %d(%d) %s%s: lohi restore field %d\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell, iaValue[jj][4]);
      }
      if (psObj->iRestoLow)
      {
         iaValue[jj][5] = psObj->iRestoLow;
         iRestoLoLo++;
         if (iDebug == 2) fprintf(fLogFile,
            "    object %d(%d) %s%s: lolo restore field %d\n",
            iiObj, jj, psObj->cNamehl, psObj->cNamell, iaValue[jj][5]);
      }

gNextValue:
      psObj++;
      psFile++;
      if (iiObj == iobj)
      {
         if (iDebug) fprintf(fLogFile,
            "       buffer %d: last obj (no. %d) handled\n",
            iiBuf, iiObj);

         piptr = (int **) psObj;
         if (*piptr == NULL)
         {
            if (iDebug) fprintf(fLogFile,
               "       buffer %d: last obj buffer\n", iiBuf);
         }
         else
         {
            iiObj = 0;
            iiBuf++;
            pinext = *piptr;
            iobj = *pinext++;
            psObj = (srawRetrList *) pinext;
            if (iDebug) fprintf(fLogFile,
               "       new buffer %d, %d objs, first: %s%s|\n",
               iiBuf, iobj, psObj->cNamehl, psObj->cNamell);
         }
      } /* iiObj == iobj */
   } /* loop over all objects */

   if (iDebug) fprintf(fLogFile,
      "    usage of restore fields: %u-%u-%u-%u-%u\n",
      iRestoTop, iRestoHiHi, iRestoHiLo, iRestoLoHi, iRestoLoLo);

   /**** copy WC entries to final destination (behind RC entries) ****/

   if (iWriteCache)
   {
      psObjSort = psObjSort0;                        /* first object */
      psFileSort = psFileSort0;                        /* first file */

      for (ii=1; ii<=iWriteCache; ii++)
      {
         memcpy((char *) psObjComp, (char *) psObjSort, iRetrList);
         memcpy(psFileComp->cFile, psFileSort->cFile, iFileList);

         psObjSort++;
         psFileSort++;
         psObjComp++;
         psFileComp++;

      } /* loop write cache entries */
   } /* (iWriteCache) */

   if ( ((iReadCache) || (iWriteCache)) && (iDebug == 2) )
   {
      psObjComp = psObjComp0;                     /* first object */
      psFileComp = psFileComp0;                     /* first file */

      if (iReadCache)
      {
         fprintf(fLogFile,
            "%d read cache entries (Comp)\n", iReadCache);
         for (ii=1; ii<=iReadCache; ii++)
         {
            fprintf(fLogFile, "   %d: obj %s%s, file %s\n",
               ii, psObjComp->cNamehl, psObjComp->cNamell,
               psFileComp->cFile);
            psObjComp++;
            psFileComp++;
         }
      }

      if (iWriteCache)
      {
         fprintf(fLogFile,
            "%d write cache entries (Comp)\n", iWriteCache);
         for (ii=1; ii<=iWriteCache; ii++)
         {
            jj = iReadCache + ii;
            fprintf(fLogFile, "   %d: obj %s%s, file %s\n",
               jj, psObjComp->cNamehl, psObjComp->cNamell,
               psFileComp->cFile);
            psObjComp++;
            psFileComp++;
         }
      }
   } /* (iReadCache || iWriteCache) && (iDebug == 2) */

   /* no more TSM files left */
   if (iReadCache + iWriteCache == iFileComp)
      goto gEndCheckObjlist;

   /*********** create compressed arrays and obj/file buffers ********/
   
   iFileSort = iFileComp - iReadCache - iWriteCache;
   psObjSort = (srawRetrList *) piolsort;            /* first object */
   psFileSort = (srawFileList *) piflsort;             /* first file */

   psObj = psObj0;                       /* 1st object in 1st buffer */
   psFile = psFile0;
   iobj = iobj0;                     /* no. of objects in 1st buffer */
   iiObj = 0;                        /* running object no. in buffer */
   iiBuf = 1;                                  /* running buffer no. */
   ii = 0;                  /* no. of objs with restore order fields */
   if (iDebug) fprintf(fLogFile,
      "DDD TSM restore values before sorting:\n");

   /* loop over all objects to select the TSM objects */
   for (jj=1; jj<=iFileAll; jj++)
   {
      iiObj++;
      if (iaValue[jj][0] < 3)
      {
         if (iDebug)
         {
            fprintf(fLogFile,
               "    %d: (%d) ignored", jj, iaValue[jj][0]);
            if (psObj->iStageFS)
               fprintf(fLogFile, " (RC)\n");
            else if (psObj->iCacheFS)
               fprintf(fLogFile, " (WC)\n");
            else
               fprintf(fLogFile, "\n");
         }

         goto gNextObjCompress;
      }

      ii++;
      iaCompValue[ii][0] = iaValue[jj][0];
      iaCompValue[ii][1] = iaValue[jj][1];
      iaCompValue[ii][2] = iaValue[jj][2];
      iaCompValue[ii][3] = iaValue[jj][3];
      iaCompValue[ii][4] = iaValue[jj][4];
      iaCompValue[ii][5] = iaValue[jj][5];

      if (iDebug) fprintf(fLogFile,
         "    %d(%d): %d %d-%d-%d-%d-%d\n", jj, ii,
         iaValue[jj][0], iaValue[jj][1], iaValue[jj][2],
         iaValue[jj][3], iaValue[jj][4], iaValue[jj][5]);

      memcpy((char *) psObjSort, (char *) psObj, iRetrList);
      memcpy(psFileSort->cFile, psFile->cFile, iFileList);
      psObjSort++;
      psFileSort++;

gNextObjCompress:
      psObj++;
      psFile++;
      if (iiObj == iobj)
      {
         if (iDebug == 2) fprintf(fLogFile,
            "       buffer %d: last obj (no. %d) handled\n",
            iiBuf, iiObj);

         piptr = (int **) psObj;
         if (*piptr == NULL)
         {
            if (iDebug == 2) fprintf(fLogFile,
               "       buffer %d: last obj buffer\n", iiBuf);
         }
         else
         {
            iiObj = 0;
            iiBuf++;
            pinext = *piptr;
            iobj = *pinext++;
            psObj = (srawRetrList *) pinext;
            if (iDebug == 2) fprintf(fLogFile,
               "       new buffer %d, %d objs, first: %s%s\n",
               iiBuf, iobj, psObj->cNamehl, psObj->cNamell);
         }
      } /* iiObj == iobj */
   } /* loop over all objects to select the TSM objects */

   if (ii != iFileSort)
   {
      fprintf(fLogFile,
         "-E- %s: inconsistent no. of TSM files: %d - %d\n",
         cModule, iFileSort, ii);
      return -2;
   }

   psObjSort = (srawRetrList *) piolsort;        /* first object */
   psFileSort = (srawFileList *) piflsort;         /* first file */

   /*************** sort top restore values in array *****************/

   if (iDebug == 2) fprintf(fLogFile,
      "DDD list of compressed TSM objs/files (ps...Sort):\n");

   for (jj=1; jj<=iFileSort; jj++)
   {
      if (iRestoTop)
         iaValTop[jj] = iaCompValue[jj][1];
      else
         iaValTop[jj] = 0;

      if (iRestoHiHi)
         iaValHiHi[jj] = iaCompValue[jj][2];
      else
         iaValHiHi[jj] = 0;

      if (iRestoHiLo)
         iaValHiLo[jj] = iaCompValue[jj][3];
      else
         iaValHiLo[jj] = 0;

      if (iRestoLoHi)
         iaValLoHi[jj] = iaCompValue[jj][4];
      else
         iaValLoHi[jj] = 0;

      if (iRestoLoLo)
         iaValLoLo[jj] = iaCompValue[jj][5];
      else
         iaValLoLo[jj] = 0;

      iaIndex[jj] = jj;

      if (iDebug == 2)
      {
         fprintf(fLogFile, "    %d: obj %s%s, file %s\n",
            jj, psObjSort->cNamehl, psObjSort->cNamell,
            psFileSort->cFile);

         psObjSort++;
         psFileSort++;
      }
   }

   iisort1 = 1;
   iisort2 = iFileSort;

   if (iRestoTop)
   {
      iRC = rawSortValues(iaValTop, iFileSort, iisort1, iisort2,
                          iaIndex, iaIndNew); 

      /* rearrange other restore fields */
      iDiff = 0;
      for (jj=iisort1; jj<=iisort2; jj++)
      {
         if (iaIndNew[jj] != iaIndex[jj])
         {
            ii1 = iaIndNew[jj];
            if (iRestoHiHi)
               iaValHiHi[jj] = iaCompValue[ii1][2];
            if (iRestoHiLo)
               iaValHiLo[jj] = iaCompValue[ii1][3];
            if (iRestoLoHi)
               iaValLoHi[jj] = iaCompValue[ii1][4];
            if (iRestoLoLo)
               iaValLoLo[jj] = iaCompValue[ii1][5];
            iDiff++;
         }
      }

      /* update index field */
      memcpy(&iaIndex[iisort1], &iaIndNew[iisort1],
             (unsigned) iFileSort*iint);

      if (iDebug == 2)
      {
         fprintf(fLogFile,
            "DDD restore order after top sorting (%d changes):\n",
            iDiff);
         for (jj=iisort1; jj<=iisort2; jj++)
         {
            fprintf(fLogFile,
               "    %d: index %d, values: %u-%u-%u-%u-%u\n",
               jj, iaIndNew[jj], iaValTop[jj],
               iaValHiHi[jj], iaValHiLo[jj],
               iaValLoHi[jj], iaValLoLo[jj]);
         }
      }

      iRestoTopValues = 1;
      iRestoTopMin = iaValTop[iisort1];
      for (jj=iisort1+1; jj<=iisort2; jj++)
      {
         if (iaValTop[jj] > iRestoTopMin)
         {
            iRestoTopMin = iaValTop[jj];
            iRestoTopValues++;
         }
      }

      if (iDebug) fprintf(fLogFile,
         "%d different top restore values\n", iRestoTopValues);

      iitop1 = iisort1;
         /* index of 1st obj with cur top restore field */

      /* loop over different top restore fields */
      for (iTopValcur=1; iTopValcur<=iRestoTopValues; iTopValcur++)
      {
         if (iTopValcur == iRestoTopValues)
            iitop2 = iFileSort;
         else
         {
            iRestoTopMin = iaValTop[iitop1];
            for (jj=iitop1+1; jj<=iFileSort; jj++)
            {
               if (iaValTop[jj] > iRestoTopMin)
               {
                  iitop2 = jj-1;
                  break;
               }

               if (jj == iFileSort)
                  iitop2 = jj;
            }
         }

         if (iitop1 == iitop2)
         {
            if (iDebug) fprintf(fLogFile,
               "    %d. top restore value %d: only one object (%d)\n",
               iTopValcur, iaValTop[iitop1], iitop1);

            if (iTopValcur == iRestoTopValues)
               break;
            else
            {
               iitop1 = iitop2 + 1;
               continue;
            }
         }
         else if (iDebug) fprintf(fLogFile,
            "    %d. top restore value %d: objs %d - %d\n",
            iTopValcur, iaValTop[iitop1], iitop1, iitop2);

         /* sort entries according to hihi: DDD not yet used */
         if (iRestoHiHi)
         {
            ;
         } /* (iRestoHiHi) */

         /* sort entries according to hilo */
         if (iRestoHiLo)
         {
            if (iitop2 - iitop1)
            {
               iRC = rawSortValues(iaValHiLo, iFileSort, iitop1, iitop2,
                          iaIndex, iaIndNew); 

               /* rearrange other restore fields */
               iDiff = 0;
               for (jj=iitop1; jj<=iitop2; jj++)
               {
                  if (iaIndNew[jj] != iaIndex[jj])
                  {
                     ii1 = iaIndNew[jj];
                     if (iRestoTop)
                        iaValTop[jj] = iaCompValue[ii1][1];
                     if (iRestoHiHi)
                        iaValHiHi[jj] = iaCompValue[ii1][2];
                     if (iRestoLoHi)
                        iaValLoHi[jj] = iaCompValue[ii1][4];
                     if (iRestoLoLo)
                        iaValLoLo[jj] = iaCompValue[ii1][5];
                     iDiff++;
                  }
               }

               /* update index field */
               ii1 = iitop2 - iitop1 + 1;
               memcpy(&iaIndex[iitop1], &iaIndNew[iitop1],
                      (unsigned) ii1*iint);

               if (iDebug == 2)
               {
                  fprintf(fLogFile,
                     "       restore order after hilo sorting (%d - %d: %d changes):\n",
                     iitop1, iitop2, iDiff);
                  for (jj=iitop1; jj<=iitop2; jj++)
                  {
                     fprintf(fLogFile,
                        "       %d: index %d, values: %u-%u-%u-%u-%u\n",
                        jj, iaIndNew[jj], iaValTop[jj],
                        iaValHiHi[jj], iaValHiLo[jj],
                        iaValLoHi[jj], iaValLoLo[jj]);
                  }
               }

               iRestoHiLoValues = 1;
               iRestoHiLoMin = iaValHiLo[iitop1];

               for (jj=iitop1+1; jj<=iitop2; jj++)
               {
                  if (iaValHiLo[jj] > iRestoHiLoMin)
                  {
                     iRestoHiLoMin = iaValHiLo[jj];
                     iRestoHiLoValues++;
                  }
               }

               if (iDebug) fprintf(fLogFile,
                  "       %d different hilo restore values\n", iRestoHiLoValues);

            } /* (iitop2 - iitop1) */ 
            else
               iRestoHiLoValues = 1;

            iihilo1 = iitop1;
               /* index of 1st obj with cur hilo restore field */

            /* loop over different hilo fields for fixed top field */
            for (iHiLoValcur=1; iHiLoValcur<=iRestoHiLoValues; iHiLoValcur++)
            {
               if (iHiLoValcur == iRestoHiLoValues)
                  iihilo2 = iitop2;
               else
               {
                  iRestoHiLoMin = iaValHiLo[iihilo1];
                  for (jj=iitop1+1; jj<=iitop2; jj++)
                  {
                     if (iaValHiLo[jj] > iRestoHiLoMin)
                     {
                        iihilo2 = jj-1;
                        break;
                     }

                     if (jj == iitop2)
                        iihilo2 = jj;
                  }
               }

               if (iihilo1 == iihilo2)
               {
                  if (iDebug) fprintf(fLogFile,
                     "       %d. hilo restore value %d: only one object (%d)\n",
                     iHiLoValcur, iaValHiLo[iihilo1], iihilo1);

                  if (iHiLoValcur == iRestoHiLoValues)
                     break;
                  else
                  {
                     iihilo1 = iihilo2 + 1;
                     continue;
                  }
               }
               else if (iDebug) fprintf(fLogFile,
                  "       %d. hilo restore value %d: objs %d - %d\n",
                  iHiLoValcur, iaValHiLo[iihilo1], iihilo1, iihilo2);

               /* sort entries according to lohi: DDD not yet used */
               if (iRestoLoHi)
               {
                  ;
               } /* (iRestoLoHi) */

               if (iRestoLoLo)
               {
                  if (iihilo2 - iihilo1)
                  {
                     iRC = rawSortValues(iaValLoLo, iFileSort,
                               iihilo1, iihilo2, iaIndex, iaIndNew); 

                     /* rearrange other restore fields */
                     iDiff = 0;
                     for (jj=iihilo1; jj<=iihilo2; jj++)
                     {
                        if (iaIndNew[jj] != iaIndex[jj])
                        {
                           ii1 = iaIndNew[jj];
                           if (iRestoTop)
                              iaValTop[jj] = iaCompValue[ii1][1];
                           if (iRestoHiHi)
                              iaValHiHi[jj] = iaCompValue[ii1][2];
                           if (iRestoHiLo)
                              iaValHiLo[jj] = iaCompValue[ii1][3];
                           if (iRestoLoHi)
                              iaValLoHi[jj] = iaCompValue[ii1][4];
                           iDiff++;
                        }
                     }

                     /* update index field */
                     ii1 = iihilo2 - iihilo1 + 1;
                     memcpy(&iaIndex[iihilo1], &iaIndNew[iihilo1],
                            (unsigned) ii1*iint);

                     if (iDebug == 2)
                     {
                        fprintf(fLogFile,
                           "          restore order after lolo sorting (%d - %d: %d changes):\n",
                           iihilo1, iihilo2, iDiff);
                        for (jj=iihilo1; jj<=iihilo2; jj++)
                        {
                           fprintf(fLogFile,
                              "          %d: index %d, values: %u-%u-%u-%u-%u\n",
                              jj, iaIndNew[jj], iaValTop[jj],
                              iaValHiHi[jj], iaValHiLo[jj],
                              iaValLoHi[jj], iaValLoLo[jj]);
                        }
                     }
                  } /* (iihilo2 - iihilo1) */
                  else
                     iRestoLoLoValues = 1;

               } /* (iRestoLoLo) */
               else
                  iRestoLoLoValues = 0;

               iihilo1 = iihilo2 + 1;

            } /* iHiLoValcur: loop over different hilo restore fields */

         } /* (iRestoHiLo */
         else
            iRestoHiLoValues = 0;

         iitop1 = iitop2 + 1;

      } /* iTopValcur: loop over different top restore fields */
   } /* (iRestoTop) */
   else
      iRestoTopValues = 0;

   psObjSort = (srawRetrList *) piolsort;            /* first object */
   psFileSort = (srawFileList *) piflsort;             /* first file */

   if (iDebug == 2)
   {
      fprintf(fLogFile,
         "DDD final restore order after sorting:\n");
      for (jj=iisort1; jj<=iisort2; jj++)
      {
         ii = iaIndex[jj];
         fprintf(fLogFile,
            "    %d: index %d, values: %u-%u-%u-%u-%u\n",
            jj, iaIndex[jj], iaCompValue[ii][1], iaCompValue[ii][2],
            iaCompValue[ii][3], iaCompValue[ii][4], iaCompValue[ii][5]);
      }
   }

   /******** copy + compress TSM entries to temp destination *********/

   psObjSort = (srawRetrList *) piolsort;            /* first object */
   psFileSort = (srawFileList *) piflsort;             /* first file */

   psObj = psObj0;                       /* 1st object in 1st buffer */
   psFile = psFile0;
   iobj = iobj0;                     /* no. of objects in 1st buffer */
   iiObj = 0;                        /* running object no. in buffer */
   iiBuf = 1;                                  /* running buffer no. */
   icount = 0;

   if (iDebug == 2) fprintf(fLogFile,
      "DDD copy TSM objects to temp destination for compression\n");

   for (jj = 1; jj <= iFileAll; jj++)
   {
      /* skip empty, RC, WC entries */
      if (iaValue[jj][0] < 3)
      {
         if (iDebug == 2)
         {
            fprintf(fLogFile, "     (%d): ignored", jj);
            if (psObj->iStageFS)
               fprintf(fLogFile, " (RC)\n");
            else if (psObj->iCacheFS)
               fprintf(fLogFile, " (WC)\n");
            else
               fprintf(fLogFile, "\n");
         }

         iiObj++;
         psObj++;
         psFile++;

         goto gNextCopy2Temp;
      }

      iiObj++; 
      icount++;

      memcpy((char *) psObjSort, (char *) psObj, iRetrList);
      memcpy(psFileSort->cFile, psFile->cFile, iFileList);

      if (iDebug == 2)
      {
         fprintf(fLogFile,
            "    %d(%d): obj %s%s (objId %u-%u) copied (Comp), retrId %u-%u-%u-%u-%u\n",
            iiObj, jj, psObjSort->cNamehl, psObjSort->cNamell,
            psObjSort->iObjHigh, psObjSort->iObjLow,
            psObjSort->iRestoHigh, psObjSort->iRestoHighHigh,
            psObjSort->iRestoHighLow, psObjSort->iRestoLowHigh,
            psObjSort->iRestoLow);
      }

      psObj++;
      psFile++;
      psObjSort++;
      psFileSort++;

      /* last object scanned */
gNextCopy2Temp:
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
               "    %d: new buffer, %d objs, first: |%s%s|\n",
               iiBuf, iobj, psObj->cNamehl, psObj->cNamell);
         }
      } /* iiObj == iobj */
   } /* loop copy TSM objects to temp destination for compression */

   if (iDebug == 2)
   {
      psObjSort = (srawRetrList *) piolsort;         /* first object */
      psFileSort = (srawFileList *) piflsort;          /* first file */
      fprintf(fLogFile,
         "DDD compressed list of TSM objects (not yet sorted):\n");

      for (jj=1; jj<=iFileSort; jj++)
      {
         fprintf(fLogFile, "    %d: obj %s%s, file %s\n",
            jj, psObjSort->cNamehl, psObjSort->cNamell, psFileSort->cFile);

         psObjSort++;
         psFileSort++;
      }
   }

   if (icount != iFileSort)
   {
      fprintf(fLogFile,
         "-E- %s: unexpected no. of objects found: %d, expected %d\n",
         cModule, icount, iFileSort);
      return -5;
   }

   /********** copy sorted TSM entries to final destination **********
    **************** behind WC entries, using index ******************/

   psObjSort = (srawRetrList *) piolsort;            /* first object */
   psFileSort = (srawFileList *) piflsort;             /* first file */

   if (iDebug == 2) fprintf(fLogFile,
      "DDD copy TSM objects in correct order to final destination\n");

   ii1 = 1;                              /* init position on 1st obj */
   for (jj = 1; jj <= iFileSort; jj++)
   {
      ii2 = iaIndex[jj];
      iDiff = ii2 - ii1;
      psObjSort += iDiff;
      psFileSort += iDiff;

      icount++;

      memcpy((char *) psObjComp, (char *) psObjSort, iRetrList);
      memcpy(psFileComp->cFile, psFileSort->cFile, iFileList);

      if (iDebug == 2)
      {
         fprintf(fLogFile,
            "    %d: TSM obj %s%s (index %d, objId %u-%u) copied (Comp), retrId %u-%u-%u-%u-%u\n",
            jj, psObjSort->cNamehl, psObjSort->cNamell, ii2,
            psObjSort->iObjHigh, psObjSort->iObjLow,
            psObjSort->iRestoHigh, psObjSort->iRestoHighHigh,
            psObjSort->iRestoHighLow, psObjSort->iRestoLowHigh, psObjSort->iRestoLow);
      }

      psObjComp++;
      psFileComp++;
      ii1 = ii2;

   } /* for (jj...) */

   iFileAll = iReadCache + iWriteCache + iFileSort;
   if (iDebug)
   {
      psObjComp = psObjComp0;                        /* first object */
      psFileComp = psFileComp0;                        /* first file */
      fprintf(fLogFile,
         "final list of all objects (compressed and sorted):\n");

      for (jj=1; jj<=iFileAll; jj++)
      {
         fprintf(fLogFile,
            "    %d: obj %s%s, file %s, objId %u-%u, retrId %u-%u-%u-%u-%u",
            jj, psObjComp->cNamehl, psObjComp->cNamell, psFileComp->cFile,
            psObjComp->iObjHigh, psObjComp->iObjLow,
            psObjComp->iRestoHigh, psObjComp->iRestoHighHigh,
            psObjComp->iRestoHighLow, psObjComp->iRestoLowHigh,
            psObjComp->iRestoLow);
         if (psObjComp->iStageFS)
            fprintf(fLogFile, " (RC)\n");
         else if (psObjComp->iCacheFS)
            fprintf(fLogFile, " (WC)\n");
         else
            fprintf(fLogFile, "\n");

         psObjComp++;
         psFileComp++;
      }
   }

gEndCheckObjlist:

   if (iSort)
   {
      memset(pcfl, 0x00, (unsigned) iFileBufComp);
      memcpy(pcfl, pcFileComp, (unsigned) iFileBufComp);
   }

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

int rawDelFile( int iSocket, srawComm *psComm)
{
   char cModule[32] = "rawDelFile";
   int iDebug = 0;

   int iFSidRC = 0;
   int iFSidWC = 0;

   int iRC;
   int iBufComm;
   char *pcc;
   void *pBuf;
   char cMsg[STATUS_LEN] = "";

   srawStatus sStatus;

   iBufComm = ntohl(psComm->iCommLen) + HEAD_LEN;
   if (iDebug) printf(
      "\n-D- begin %s: delete file %s%s%s\n",
      cModule, psComm->cNamefs, psComm->cNamehl, psComm->cNamell);

   if (psComm->iStageFSid)
      iFSidRC = ntohl(psComm->iStageFSid);
   if (psComm->iFSidWC)
      iFSidWC = ntohl(psComm->iFSidWC);

   if (iDebug)
   {
      printf("    object %s%s%s found (objId %u-%u)", 
         psComm->cNamefs, psComm->cNamehl, psComm->cNamell, 
         ntohl(psComm->iObjHigh), ntohl(psComm->iObjLow));
      if (iFSidRC) printf(
         ", on %s in read cache FS %d\n", psComm->cNodeRC, iFSidRC);
      else
         printf( "\n"); 
      if (iFSidWC) printf(
         "     on %s in write cache FS %d\n", psComm->cNodeWC, iFSidWC);
   }

   psComm->iAction = htonl(REMOVE); 

   pcc = (char *) psComm;
   if ( (iRC = send( iSocket, pcc, (unsigned) iBufComm, 0 )) < iBufComm)
   {
      if (iRC < 0) printf(
         "-E- %s: sending delete request for file %s (%d byte)\n",
         cModule, psComm->cNamell, iBufComm);
      else printf(
         "-E- %s: delete request for file %s (%d byte) incompletely sent (%d byte)\n",
         cModule, psComm->cNamell, iBufComm, iRC);
      if (errno)
         printf("    %s\n", strerror(errno));
      perror("-E- sending delete request");

      return -1;
   }

   if (iDebug) printf(
      "    delete command sent to server (%d bytes), look for reply\n",
      iBufComm);

   /******************* look for reply from server *******************/

   pcc = (char *) &sStatus;
   iRC = rawRecvStatus(iSocket, pcc);
   if (iRC != HEAD_LEN)
   {
      if (iRC < HEAD_LEN) printf(
         "-E- %s: receiving status buffer\n", cModule);
      else
      {
         printf("-E- %s: message received from server:\n", cModule);
         printf("%s", sStatus.cStatus);
      }
      if (iDebug)
         printf("\n-D- end %s\n\n", cModule);

      return(-1);
   }

   if (iDebug)
      printf("    status (%d) received from server (%d bytes)\n",
             sStatus.iStatus, iRC);

   printf("-I- Object %s%s%s successfully deleted",
          psComm->cNamefs, psComm->cNamehl, psComm->cNamell);
   if (iFSidRC)
      printf( " (staged)\n"); 
   else
      printf( "\n"); 
   if (iFSidWC)
      printf( "    was in write cache\n"); 

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

int rawDelList( int iSocketMaster,
                            /* socket for connection to entry server */
                int iDataMover,                /* no. of data movers */
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
   int iobj0, iobj;
   int iobjBuf;

   srawFileList *psFile, *psFile0;           /* files to be archived */
   srawRetrList *psObj;                  /* objects already archived */
   srawDataMoverAttr *pDataMover;              /* current data mover */

   char *pcc, *pdelim;

   bool_t bDelete, bDelDone;
   int **piptr;                  /* points to pointer to next buffer */

   int ii, jj, kk;
   int iRC, idel;

   pcfl = *pcFileList;
   pifl = (int *) pcfl;
   ifile = pifl[0];                            /* number of files */
   psFile = (srawFileList *) ++pifl;  /* points now to first file */
   psFile0 = psFile;
   pifl--;                     /* points again to number of files */

   if (iDebug)
   {
      printf("\n-D- begin %s\n", cModule);
      printf("    initial %d files, first file %s\n",
             ifile, psFile0->cFile);
   } 

   pDataMover = pDataMover0;
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
         printf("    buffer %d: %d objects, first obj %s%s (server %d)\n",
                iobjBuf, iobj, psObj->cNamehl, psObj->cNamell, psObj->iATLServer);

      psComm->iAction = htonl(REMOVE);
      for (ii=1; ii<=iobj; ii++)    /* loop over objects in buffer */
      {
         iobj0++;
         pcc = (char *) psObj->cNamell;
         pcc++;                          /* skip object delimiter  */

         if (iDebug) printf(
            "    obj %d: %s%s, objId %d-%d\n",
            ii, psObj->cNamehl, psObj->cNamell,
            psObj->iObjHigh, psObj->iObjLow);

         bDelete = bFalse;
         psFile = psFile0;
         for (jj=1; jj<=ifile; jj++)                  /* file loop */
         {
            if (iDebug) printf(
               "    file %d: %s\n", jj, psFile->cFile);

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

            iRC = strcmp(pdelim, pcc);
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
               printf("    matching file %d: %s, obj %d: %s%s",
                      jj, psFile->cFile, ii, psObj->cNamehl, psObj->cNamell);
               if (psObj->iStageFS)
                  printf(", on DM %s in StageFS %d\n",
                         psObj->cMoverStage, psObj->iStageFS);
               else if (psObj->iCacheFS)
               {
                  printf(", on DM %s in ArchiveFS %d\n",
                         psObj->cMoverStage, psObj->iCacheFS);
                  printf("    archived at %s by %s\n",
                         psObj->cArchiveDate, psObj->cOwner);
               }
               else
                  printf(" (not in disk pool)\n");
            }

            psComm->iObjHigh = htonl(psObj->iObjHigh);
            psComm->iObjLow = htonl(psObj->iObjLow);
            psComm->iATLServer = htonl(psObj->iATLServer);

            if (psObj->iStageFS)
            {
               psComm->iPoolIdRC = htonl(psObj->iPoolId);
               psComm->iStageFSid = htonl(psObj->iStageFS);
               strcpy(psComm->cNodeRC, psObj->cMoverStage);
            }
            else
            {
               psComm->iPoolIdRC = htonl(0);
               psComm->iStageFSid = htonl(0);
               strcpy(psComm->cNodeRC, "");
            }

            if (psObj->iCacheFS)
            {
               if (psObj->iStageFS)
                  psComm->iPoolIdWC = htonl(0); /* WC poolId unavail */
               else
                  psComm->iPoolIdWC = htonl(psObj->iPoolId);
               psComm->iFSidWC = htonl(psObj->iCacheFS);
               strcpy(psComm->cNodeWC, psObj->cMoverCache);
            }
            else
            {
               psComm->iPoolIdWC = htonl(0);
               psComm->iFSidWC = htonl(0);
               strcpy(psComm->cNodeWC, "");
            }

            iRC = rawGetLLName(psFile->cFile,
                               pcObjDelim, psComm->cNamell);

            if (iDataMover > 1)
            {
               if ((strcmp(pDataMover->cNode,psObj->cMoverStage) == 0) ||
                   (strlen(psObj->cMoverStage) == 0))  /* not staged */
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
                                psObj->cMoverStage) == 0)
                        break;
                     pDataMover++;
                  }

                  if (kk > iDataMover)
                  {
                     printf("-E- %s: data mover %s not found in list\n",
                            cModule, psObj->cMoverStage);
                     return -1;
                  }

                  iSocket = pDataMover->iSocket;
                  if (iDebug) printf(
                     "    new data mover %s, socket %d\n",
                     pDataMover->cNode, iSocket);
               }
            } /* (iDataMover > 1) */

            iRC = rawDelFile(iSocketMaster, psComm);
            if (iRC)
            {
               if (iDebug)
                  printf("    rawDelFile: rc = %d\n", iRC);
               if (iRC < 0)
               {
                  printf("-E- %s: file %s could not be deleted\n",
                         cModule, psFile->cFile);
                  if (iDebug)
                     printf("-D- end %s\n\n", cModule);

                  return -1;
               }
               /* else: object not found, ignore */

            } /* (iRC) */

            idel++;

         } /* if (bDelete) */
         else if (iDebug) printf(
            "    file %s: obj %s%s not found in gStore\n",
            psFile0->cFile, psObj->cNamehl, psObj->cNamell);

         psObj++;

      } /* loop over objects in query buffer (ii) */

      piptr = (int **) psObj;
      if (*piptr == NULL) bDelDone = bTrue;
      else piol = *piptr;

   } /* while (!bDelDone) */

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return(idel);

} /* rawDelList */

/**********************************************************************
 * rawGetFilelistEntries:
 *    get filelist entries from input file
 *    if global path specified, add to file name
 *    uses external buffer ptr piEntryList provided by caller
 *    replaces '%' (historical) by '?' (supported by TSM)
 *    removes tape file specifications
 *    removes duplicate entries
 *    orders: generic file names first
 *
 * created 22. 5.2003 by Horst Goeringer
 **********************************************************************
 */

int rawGetFilelistEntries( char *pcFileName,
                           int *piDataFS,
                           char *pcDataFS,
                           int *piEntries,
                           int *piGlobalDir)
{
   char cModule[32] = "rawGetFilelistEntries";
   int iDebug = 0;

   char cMsg[STATUS_LEN] = "";
   int ii, jj;
   int iDelCount = 0;
   int iFilesIgnored = 0;   /* =1: no. of files with specific subdir */
   int iGlobalDir = 0;       /* =1: global dir in 1st line specified */
   int iDataFS = 0;                   /* =1: path in central data FS */
   int imax = MAX_FULL_FILE - 1;
   char cPath[MAX_FULL_FILE] = ""; 
   char cEntry[MAX_FULL_FILE] = "";
   char cTemp[MAX_FULL_FILE] = "";
   char cFileName[MAX_FULL_FILE] = "";
   char cQualifier[16] = ".filelist";
   const char *pcLeading = "@";
   char *pcc;
   char *ploc;
   char *pccT = NULL; 
   char *pccE; 

   int iRemove = 0;
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
   srawFileList *pFilelist,                  /* first filelist entry */
                *pFilelistc,               /* current filelist entry */
                *pFilelistco,   /* current filelist entry old buffer */
                *pFilelistc0;   /* current filelist entry for remove */

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   iDataFS = *piDataFS;
   piFilelist = piEntryList;             /* copy of external pointer */
   iPathPrefix = strlen(cPathPrefix);              /* from rawclin.h */

   if ((int) strlen(pcFileName) >= imax)
   {
      fprintf(fLogFile,
         "-E- %s: file name too long (max %d)\n",
         cModule, --imax);
      iError = -1;
      goto gErrorFilelist;
   }
   strcpy(cFileName, pcFileName);                      /* local copy */

   pccT = (char *) strrchr(pcFileName, *pcLeading);
   if (pccT)
   {
      pccE = (char *) strrchr(pccT, *pcObjDelim);
      if (pccE)                             /* '@' only in path name */
         pccT = NULL;
   }

   if (pccT == NULL)
   {
      if (iDebug) fprintf(fLogFile,
         "    no leading '%s' in file name %s\n", pcLeading, pcFileName);
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
         "    no trailing %s in file name %s\n", cQualifier, pcFileName);
      goto gEndFilelist;
   }

   if (iDebug) fprintf(fLogFile,
      "    %s is a filelist\n", cFileName);

   fiFile = fopen(pcFileName, "r");
   if (fiFile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening filelist %s\n",
         cModule, pcFileName);
      if (errno)
         fprintf(fLogFile, "    %s\n", strerror(errno));
      perror("-E- opening filelist");

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
      "DDD piFilelist %p, *piFilelist %d\n",
      piFilelist, *piFilelist);

   pFilelist = (srawFileList *) &(piFilelist[1]);
                                          /* skip max no. of entries */
   pFilelistc = pFilelist;                          /* current entry */
   pccE = cEntry;
   iEntries = 0;
   for(;;)
   {
      pccE = fgets(cEntry, imax, fiFile);

      if ( (pccE != NULL) && (strlen(pccE) > 1) )
      {
         /* remove blanks, line feeds, and so on */
         ii = strlen(pccE);
         memset(cTemp, '\0', strlen(cTemp)); 
         pccT = cTemp;
         iDelCount = 0;
         while (ii-- > 0)
         {
            if ( (*pccE != '\0') && (*pccE != '\n') && (*pccE != '\r') &&
                 (*pccE != ' ') )
            {
               strncpy(pccT, pccE, 1);
               pccT++;
            }
            else
               iDelCount++;

            pccE++;

         }
         strncpy(pccT, "\0", 1);           /* terminates copied name */

         if (iDebug == 2) fprintf(fLogFile,
            "'%s' -> '%s': %d chars removed\n", cEntry, cTemp, iDelCount);

         /* check for path prefix */
         if (iEntries == 0)
         {
            if (strncmp(cTemp, cPathPrefix, (unsigned) iPathPrefix) == 0)
            {
               iGlobalDir = 1;

               if (iDebug) fprintf(fLogFile,
                  "    path prefix '%s' in first filelist entry found\n",
                  cPathPrefix);

               pccT = cTemp;
               for (ii=1; ii<=iPathPrefix; ii++)
               { pccT++; }

               if (strlen(pccT) == 0)
               {
                  fprintf(fLogFile,
                     "-W- no path name found after path prefix: %s\n",
                     cEntry);
                  continue;
               }

               ii = strlen(pccT) - 1;
               pcc = pccT;
               pcc += ii;
               if (strncmp(pcc, "/", 1) == 0)        /* trailing '/' */
                  strncpy(cPath, pccT, (unsigned) ii);
               else
                  strcpy(cPath, pccT);

               if (iDebug) fprintf(fLogFile,
                  "    path '%s' for files in filelist\n", cPath);

               if ( (strncmp(cPath, cDataFSHigh1,
                             strlen(cDataFSHigh1)) == 0) ||
                    (strncmp(cPath, cDataFSHigh2,
                             strlen(cDataFSHigh2)) == 0) )
               {
                  iDataFS = 1;
                  strcpy(pcDataFS, cPath);

                  if (iDebug) fprintf(fLogFile,
                     "    central data FS path %s\n", cPath);
               }

               continue;

            } /* path prefix found */
         } /* (iEntries == 0) */

         if (strncmp(cTemp, cPathPrefix, (unsigned) iPathPrefix) == 0)
         {
            fprintf(fLogFile,
               "-E- only one path specification in file list allowed, %s and following files ignored\n",
               cTemp);
            break;
         }

         /* replace '%' (historical) by '?' (supported by TSM) */
         while ( (ploc = strchr(cTemp, *pcPerc)) != NULL)
         {
            if (iDebug)
               fprintf(fLogFile, "-W- replace %s", cTemp);
            *ploc = *pcQM;
            if (iDebug)
               fprintf(fLogFile, " by %s\n", cTemp);
         }

         if ( (iEntries == iMaxEntries) &&             /* reallocate */
              (iEntries > 0) )
         {
            iMaxEntries += iMaxEntries;
            if (iDebug) fprintf(fLogFile,
               "    entry buffer full, reallocate: max %d entries\n",
               iMaxEntries);

            iSizeBuffer = iMaxEntries*MAX_FULL_FILE + sizeof(int);
            piFilelisto = piFilelist;
            piFilelist = (int *) calloc(
               (unsigned) iSizeBuffer, sizeof(char) );
            if (piFilelist == NULL)
            {
               fprintf(fLogFile,
                  "-E- %s: reallocating filelist buffer (size %d)\n",
                  cModule, iSizeBuffer);
               if (errno)
                  fprintf(fLogFile, "    %s\n", strerror(errno));
               perror("-E- reallocating filelist buffer");

               iError = -1;
               goto gErrorFilelist;
            }

            if (iDebug) fprintf(fLogFile,
               "    filelist entry buffer reallocated (size %d)\n",
               iSizeBuffer);

            *piFilelist = iMaxEntries;
            if (iDebug == 2) fprintf(fLogFile,
               "DDD piFilelist %p, *piFilelist %d\n",
                piFilelist, *piFilelist);

            pFilelistc = (srawFileList *) &(piFilelist[1]);   /* new */
            pFilelistco = pFilelist;                   /* old buffer */
            pFilelist = pFilelistc;              /* first new buffer */

            for (ii=1; ii<=iEntries; ii++)
            {
               if (iDebug == 2) fprintf(fLogFile,
                  "DDD pFilelistc %p\n", pFilelistc);
               strcpy(pFilelistc->cFile, pFilelistco->cFile);
               pFilelistc++;
               pFilelistco++;
            }

            if (iDebug) fprintf(fLogFile,
               "    %d old entries copied to new buffer, next:\n",
               iEntries);

            if (iDebug == 2) fprintf(fLogFile,
               "DDD free piFilelisto %p\n", piFilelisto);
            free(piFilelisto);
            piFilelisto = piFilelist;

         } /* (iEntries == iMaxEntries) */

         if (iDebug == 2) fprintf(fLogFile,
            "DDD pFilelistc %p\n", pFilelistc);

         if ( (ploc = strchr(cTemp, *pcObjDelim)) != NULL)
         {
            iFilesIgnored++;
            if (iFilesIgnored == 1) fprintf(fLogFile,
               "-W- file-specific path not yet implemented in filelist\n"); 
            fprintf(fLogFile, "-W- %s ignored\n", cTemp);
            continue;
         }

         if (iGlobalDir)
         {
            strcpy(pFilelistc->cFile, cPath);
            strncat(pFilelistc->cFile, "/", 1);
         }
         strcat(pFilelistc->cFile, cTemp);
         iEntries++;

         if (iDebug) fprintf(fLogFile,
            "    %3d: %s \n", iEntries, pFilelistc->cFile);

         pFilelistc++;
      }
      else
         break;

   } /* read loop */

   *piGlobalDir = iGlobalDir;                          /* for caller */

   pFilelistc = pFilelist;
   if (iDebug)
   {
      fprintf(fLogFile, "    after allocation:\n");
      for (ii=1; ii<=iEntries; ii++)
      {
         fprintf(fLogFile,
            "    %3da: %s \n", ii, pFilelistc->cFile);
         pFilelistc++;
      }
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
   else
   {
      if (iDebug)
         fprintf(fLogFile, "    no valid entries found in list\n");
      goto gEndFilelist;
   }
   
   pFilelistc = pFilelist;
   if (iDebug)
   {
      fprintf(fLogFile, "    after removal of duplicates:\n");
      for (ii=1; ii<=iEntries; ii++)
      {
         fprintf(fLogFile,
            "    %3db: %s \n", ii, pFilelistc->cFile);
         pFilelistc++;
      }
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

   *piFilelist = iEntries;                           /* store result */

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

   /* reorder: generic names first */
   if ( (iGeneric) && (iSingle) )
   {
      piFilelisto = piFilelist;

      /* create buffer for sorted entries */
      iSizeBuffer = iEntries*MAX_FULL_FILE + sizeof(int);
      piFilelist = (int *) calloc((unsigned) iSizeBuffer, sizeof(char));
      if (piFilelist == NULL)
      {
         fprintf(fLogFile,
            "-E- %s: allocating ordered filelist buffer (size %d)\n",
            cModule, iSizeBuffer);
         if (errno)
            fprintf(fLogFile, "    %s\n", strerror(errno));
         perror("-E- allocating ordered filelist buffer");

         iError = -1;
         goto gErrorFilelist;
      }

      if (iDebug) fprintf(fLogFile,
         "    ordered filelist buffer allocated (size %d)\n",
         iSizeBuffer);

      if (iDebug == 2) fprintf(fLogFile,
         "DDD piFilelist %p, *piFilelist %d\n",
         piFilelist, *piFilelist);

      pFilelistc = (srawFileList *) &(piFilelist[1]);
      pFilelist = pFilelistc;
      pFilelistco = (srawFileList *) &(piFilelisto[1]);
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

      pFilelistco = (srawFileList *) &(piFilelisto[1]);
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
   {
      fprintf(fLogFile, "    after reordering:\n");
      for (ii=1; ii<=iEntries; ii++)
      {
         fprintf(fLogFile,
            "    %3dc: %s \n", ii, pFilelistc->cFile);
         pFilelistc++;
      }
   }

gEndFilelist:
   *piEntries = iEntries;
   iError = 0;

   if ( (iDebug == 2) && (pccT) ) fprintf(fLogFile,
      "DDD piFilelist %p, *piFilelist %d\n",
      piFilelist, *piFilelist);

gErrorFilelist:
   if (fiFile)
   {
      iRC = fclose(fiFile);
      if (iRC)
      {
         fprintf(fLogFile, "-E- %s: closing filelist %s\n",
            cModule, pcFileName);
         if (errno)
            fprintf(fLogFile, "    %s\n", strerror(errno));
         perror("-E- closing filelist");
      }
   }

   *piDataFS = iDataFS;
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
   char cModule[32] = "rawGetWSInfo";
   int iDebug = 0;

   int iRC;
   int iSocket;
   int iPoolInfo;               /* =1: provide info on stage pool(s) */
   int iPrintPoolInfo;            /* =1: print info on stage pool(s) */
   int iWorkSpaceInfo;              /* =1: provide info on workspace */
   char cMsg[STATUS_LEN] = "";

   int iAction;
   int iATLServer = 0;          /* =1: aixtsm1, =2: gsitsma, =0: all */

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
   int iPoolRandFree = 0;       /* free space of random pool (MByte) */
   int iPoolRandUsed;           /* used space of random pool (MByte) */
   int iRandomExcess = 0;
          /* unused space of other pools used in random pool (MByte) */
   int iPoolTempAvail = 0;
   int iPoolTempMax = 0;        /* overall size of temp pool (MByte) */
   int iPoolTempMaxWS = 0;       /* max WS size in temp pool (MByte) */
   int iPoolTempFree = 0;         /* free space of temp pool (MByte) */
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
   int iWorkStatus = 0;       /* status of work space sent by server */


   int *piBuffer;
   char *pcc;
   char cMsgPref[8] = "";
   char cMisc[128] = "";
   char pcGenFile[MAX_FULL_FILE] = "";   /* full (generic) file name */

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
      iRC = rawRecvHead(iSocket, pcc);
      if (iRC <= 0)
      {
        printf("-E- receiving header pool status from master server\n");
        return -1;
      }

      iIdent = ntohl(pPoolInfo->iIdent);
      iStatus = ntohl(pPoolInfo->iPoolNo);
      iAttrLen = ntohl(pPoolInfo->iStatusLen);

      if (iDebug) printf(
         "    header pool status received (%d bytes): ident %d, status %d, attrlen %d\n",
         iRC, iIdent, iStatus, iAttrLen);

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
                  iRC = rawRecvError(iSocket, iAttrLen, pcc);
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
         if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0)
         {
            if (iRC < 0) printf(
               "-E- %s: receiving pool info\n", cModule);
            else
            {
               ii = iBufPool - iBuf;
               printf(
                  "-E- %s: connection to entry server broken, %d byte of pool info (%d byte) received\n",
                  cModule, ii, iBufPool);
            }
            if (errno)
               printf("    %s\n", strerror(errno));
            perror("-E- receiving pool info");

            return -1;
         }

         iBuf -= iRC;
         pcc += iRC;

      } /* while(iBuf > 0) */

      if (iDebug) printf(
         "    stage pool status received (%d bytes)\n", iBufPool);

      iATLServer = 1;      /* DDD second or both not yet implemented */
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
                  "-I- overall HW space ATL server%d %8d MByte, free %8d MByte\n",
                  iATLServer,
                  ntohl(pPoolInfoData->iMaxSizeHW), iHardwareFree);

               if ( (iPoolId == iPoolMode) || (iPoolId == 0) )
               {
                  printf("    %s: used for 'gstore retrieve'\n",
                     pPoolInfoData->cPoolName);

                  iPoolRandUsed = ntohl(pPoolInfoData->iMaxSize) -
                                  iPoolRandFree;
                  if (iPoolRandFree < 0)
                     iRandomExcess = -iPoolRandFree;

                  printf(
                     "       used space  %8d MByte, %d files stored\n",
                     iPoolRandUsed, ntohl(pPoolInfoData->iFiles));

                  if (iAction == QUERY_POOL)
                  {
                     if (iHardwareFree > 0) printf(
                        "       free space (%8d MByte) shared with StagePool\n",
                        iHardwareFree);
                     printf("       no min file lifetime\n");
                  }

               } /* (iPoolId == iPoolMode) || (iPoolId == 0) */
            } /* (iPoolMode == 1) */

            if (iPoolMode == 2)
            {
               if ( (iPoolId == iPoolMode) || (iPoolId == 0) )
               {
                  if (iPrintPoolInfo) printf(
                     "    %s: used for 'gstore stage'\n",
                     pPoolInfoData->cPoolName);
                  iPoolTempAvail = ntohl(pPoolInfoData->iFileAvail);

                  iPoolTempUsed = ntohl(pPoolInfoData->iMaxSize) -
                                  iPoolTempFree;
                  iPoolTempFree = iHardwareFree;    /* free HW size */
                  if (iPoolTempFree < 0)
                     iPoolTempFree = 0;

                  if (iHardwareFree < iPoolTempFree)
                     iTempSizeUnavail = iPoolTempFree - iHardwareFree;

                  if ( (iPoolTempFree >= 0) && (iPrintPoolInfo) )
                  {
                     printf( "       used space  %8d MByte, %d files stored\n",
                        iPoolTempUsed, ntohl(pPoolInfoData->iFiles));
                     if ( (iAction == QUERY_POOL) &&
                          (iPoolTempFree > 0) ) printf(
                        "       free space (%8d MByte) shared with RetrievePool\n",
                        iHardwareFree);
                  }
                  if (iPrintPoolInfo) printf(
                     "       min file availability of %d days guaranteed\n",
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

      iRC = rawRecvHead(iSocket, pcc);
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
                  iRC = rawRecvError(iSocket, iAttrLen, pcc);
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
         if ( (iRC = recv( iSocket, pcc, (unsigned) iBuf, 0 )) <= 0)
         {
            if (iRC < 0) printf(
               "-E- %s: receiving work space info (%d byte)\n",
               cModule, iAttrLen);
            else
            {
               ii = iAttrLen - iBuf;
               printf("-E- %s: connection to sender broken, %d byte of work space info (%d byte) received\n",
                  cModule, ii, iAttrLen);
            }
            if (errno)
               printf("    %s\n", strerror(errno));
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
                     "    to get a guaranteed availability of %d days on disk pool use 'gstore stage'\n",
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
            if ( (iAction == QUERY_WORKSPACE) ||
                 (iWorkStatus == 2) )
            {
               strcpy(cMsgPref, "-W-");
               printf("%s staging requested workspace '%s' completely may fail\n",
                  cMsgPref, pcGenFile);
            }
            else
            {
               strcpy(cMsgPref, "-E-");
               printf("%s requested workspace '%s' cannot be staged:\n",
                  cMsgPref, pcGenFile);
            }

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

               if (iWorkStatus == 3) printf(
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
               printf("    -> use 'gstore stage'\n");

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
               strcpy(cMisc, "must be initiated (gstore stage)");
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
 * rawGetFullFile: get full file name from generic input and ll name
 *    returns full file name
 *
 * created 18. 3.1996, Horst Goeringer
 ********************************************************************
 */

char *rawGetFullFile(char *pcFile, char *pcNamell)
{

   char cModule[32] = "rawGetFullFile";
   int iDebug = 0;

   char cname1[MAX_FULL_FILE] = "";    /* name structure assumed:    */ 
   int ilen;
   int iRC;

   char *pdelim=NULL, *pdelim2=NULL;
   char *pc, *pcll;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

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
                       = 11: debug print mode, stage status unknown
                     */
{
   char cModule[32]="rawQueryPrint";
   char ctype[8] = "";
   char cMClass[12] = "";
   char cPath[MAX_OBJ_HL];
   char cStatus[16] = "";

   int ii;
   int iStage = 0;                         /* StagePool (read cache) */
   int iCache = 0;                      /* ArchivePool (write cache) */ 
   int iATLServer = 0;     /* =1: aixtsm1(AIX), =2: gsitsma(Windows) */
   int iFileType = -1;
   int iMediaClass = 0;

   unsigned long *plFileSizeC;                    /* 8 byte filesize */
   unsigned long lFileSize;                       /* 8 byte filesize */
   int iVersionObjAttr = 0;
          /* version no. of srawObjAttr:
             =3: 288 byte, 2 restore fields
             =4: 304 byte, 5 restore fields, ATL server no.
             =5: 384 byte, maior enhancement */

   iVersionObjAttr = ntohl(pQAttr->iVersion);
   if ( (ipMode == 1) || (ipMode == 11) ) fprintf(fLogFile,
      "\n-D- begin %s: objAttr V%d\n", cModule, iVersionObjAttr);

   if ( (iVersionObjAttr != VERSION_SRAWOBJATTR) &&
        (iVersionObjAttr != VERSION_SRAWOBJATTR-1) )
   {
      fprintf(fLogFile,
         "-E- %s: invalid cacheDB entry version %d\n",
         cModule, iVersionObjAttr);

      return;
   }

   iFileType = ntohl(pQAttr->iFileType);
   iMediaClass = ntohl(pQAttr->iMediaClass);
   iATLServer = ntohl(pQAttr->iATLServer);

   switch(iFileType)
   {
      case FIXED_INTS4:
         strcpy(ctype, "FB");
         break;
      case STREAM:
         strcpy(ctype, "U ");
         break;
      default:
         fprintf(fLogFile, "-E- Invalid record format %d found\n",
            iFileType);
         break;
   }

   switch(iMediaClass)
   {
      case MEDIA_FIXED:
         strcpy(cMClass, "DISK ");
         break;
      case MEDIA_LIBRARY:
         strcpy(cMClass, "TAPE ");
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
      case GSI_MEDIA_INCOMPLETE:
         strcpy(cMClass, "STAGE*");
         strcpy(cStatus, "still staging");
         iStage = 1;
         break;
      case GSI_STAGE_INVALID:
         strcpy(cMClass, "STAGE*");
         strcpy(cStatus, "staging failed");
         iStage = 1;
         break;
      case GSI_MEDIA_CACHE:                       /* GSI write cache */
      case GSI_CACHE_LOCKED:
         strcpy(cMClass, "CACHE");
         strcpy(cStatus, "cached");
         iCache = 1;
         break;
      case GSI_CACHE_INCOMPLETE:
      case GSI_CACHE_COPY:
         strcpy(cMClass, "CACHE*");
         strcpy(cStatus, "still caching");
         iCache = 1;
         break;
      default:
         fprintf(fLogFile, "-E- Invalid media class %d found\n",
            iMediaClass);
         break;
   }

   if (iATLServer)
   {
      if (iATLServer < 0)
         iATLServer = -iATLServer;

      ii = MAX_ATLSERVER;
      if (ii > 1)
         sprintf(cMClass, "%s%d", cMClass, iATLServer);
      else
         sprintf(cMClass, "%s", cMClass);
   }

   if ( (ipMode == 10) || (ipMode == 11) )
      strcat(cMClass, "??");             /* stage status not checked */

   if (strlen(pQAttr->cDateCreate) == 16)       /* no trailing blank */
      strcat(pQAttr->cDateCreate, " ");
   strcpy(cPath, (char *) rawGetPathName(pQAttr->cNamehl));
   fprintf(fLogFile, "%s%s%s  %s  %s  %s", 
      pQAttr->cNamefs, cPath, pQAttr->cNamell, 
      pQAttr->cOwner, pQAttr->cDateCreate, cMClass);

   pQAttr->iFileSize = ntohl(pQAttr->iFileSize);
   pQAttr->iFileSize2 = ntohl(pQAttr->iFileSize2);
   plFileSizeC = (unsigned long *) &(pQAttr->iFileSize);
   lFileSize = *plFileSizeC;

   if (lFileSize)
   {
      if ( (pQAttr->iFileSize2 == 0) ||
           (sizeof(long) == 8) )
         fprintf(fLogFile, "  %12lu", lFileSize); 
      else
         fprintf(fLogFile, "    %s   ", cTooBig);
   }

   /* reconvert to net format */
   pQAttr->iFileSize = htonl(pQAttr->iFileSize);
   pQAttr->iFileSize2 = htonl(pQAttr->iFileSize2);

   if (iStage)
      fprintf(fLogFile, "  %s  %s", pQAttr->cNode, pQAttr->cStageUser);
   if (iCache)
      fprintf(fLogFile, "  %s", pQAttr->cNode);
   fprintf(fLogFile, "\n");

   if ( (ipMode == 1) || (ipMode == 11) )
   {
      if (ntohl(pQAttr->iFS))
         fprintf(fLogFile, "    %s on data mover %s, FS %d (poolId %d)\n",
            cStatus, pQAttr->cNode, ntohl(pQAttr->iFS),
            ntohl(pQAttr->iPoolId));
      fprintf(fLogFile,
         "    obj-Id: %u-%u, restore order: %u-%u-%u-%u-%u\n",
         ntohl(pQAttr->iObjHigh),
         ntohl(pQAttr->iObjLow),
         ntohl(pQAttr->iRestoHigh),
         ntohl(pQAttr->iRestoHighHigh),
         ntohl(pQAttr->iRestoHighLow),
         ntohl(pQAttr->iRestoLowHigh),
         ntohl(pQAttr->iRestoLow) );
      fprintf(fLogFile,
         "    owner: %s, OS: %.8s, mgmt-class: %s, file set %d\n",
         pQAttr->cOwner,
         pQAttr->cOS, pQAttr->cMgmtClass, ntohl(pQAttr->iFileSet));
   }

   if ( (ipMode == 1) || (ipMode == 11) )
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

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
        int iOut,                            /* length output string */
        char *pcOut) 
{
   char cModule[32] = "rawQueryString";
   int iDebug = 0;

   int iMsg = 0;
   char ctype[8] = "";
   char cMClass[12] = "";
   char cPath[MAX_OBJ_HL];
   char cMsg[4096] = "";
   char cMsg1[1024] = "";

   int ii;
   int iStage = 0;                         /* StagePool (read cache) */
   int iCache = 0;                      /* ArchivePool (write cache) */
   int iATLServer;         /* =1: aixtsm1(AIX), =2: gsitsma(Windows) */
   int iFileType = -1;
   int iMediaClass = 0;

   unsigned long *plFileSizeC;                    /* 8 byte filesize */
   unsigned long lFileSize;                       /* 8 byte filesize */
   int iVersionObjAttr = 0;
          /* version no. of srawObjAttr:
             =3: 288 byte, 2 restore fields
             =4: 304 byte, 5 restore fields, ATL server no.
             =5: 384 byte, maior enhancement */

   iVersionObjAttr = ntohl(pQAttr->iVersion);
   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: version ObjAttr %d\n", cModule, iVersionObjAttr);

   if ( (iVersionObjAttr != VERSION_SRAWOBJATTR) &&
        (iVersionObjAttr != VERSION_SRAWOBJATTR-1) )
   {
      fprintf(fLogFile,
         "-E- %s: invalid cacheDB entry version %d\n",
         cModule, iVersionObjAttr);

      return 0;
   }

   iFileType = ntohl(pQAttr->iFileType);
   iMediaClass = ntohl(pQAttr->iMediaClass);
   iATLServer = ntohl(pQAttr->iATLServer);

   switch(iFileType)
   {
      case FIXED_INTS4:
         strcpy(ctype, "FB");
         break;
      case STREAM:
         strcpy(ctype, "U ");
         break;
      default:
         fprintf(fLogFile, "-E- Invalid record format %d found\n",
            iFileType);
         break;
   }

   switch(iMediaClass)
   {
      case MEDIA_FIXED:
         strcpy(cMClass, "DISK ");
         break;
      case MEDIA_LIBRARY:
         strcpy(cMClass, "TAPE ");
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
      case GSI_MEDIA_INCOMPLETE:
      case GSI_STAGE_INVALID:
         strcpy(cMClass, "STAGE*");
         break;
      case GSI_MEDIA_CACHE:                       /* GSI write cache */
      case GSI_CACHE_LOCKED:
         strcpy(cMClass, "CACHE");
         iCache = 1;
         break;
      case GSI_CACHE_INCOMPLETE:
      case GSI_CACHE_COPY:
         strcpy(cMClass, "CACHE*");
         iCache = 1;
         break;
      default:
         fprintf(fLogFile, "-E- Invalid media class %d found\n",
            iMediaClass);
         break;
   }

   if (iATLServer)
   {
      if (iATLServer < 0)
         iATLServer = -iATLServer;

      ii = MAX_ATLSERVER;
      if (ii > 1)
         sprintf(cMClass, "%s%d", cMClass, iATLServer);
      else
         sprintf(cMClass, "%s", cMClass);
   }
   if ( (ipMode == 10) || (ipMode == 11) )
      strcat(cMClass, "??");             /* stage status not checked */

   if (strlen(pQAttr->cDateCreate) == 16)       /* no trailing blank */
      strcat(pQAttr->cDateCreate, " ");
   strcpy(cPath, (char *) rawGetPathName(pQAttr->cNamehl));

   sprintf(cMsg, "%s%s%s  %s  %s  %s", 
      pQAttr->cNamefs, cPath, pQAttr->cNamell, 
      pQAttr->cOwner, pQAttr->cDateCreate, cMClass);

   pQAttr->iFileSize = ntohl(pQAttr->iFileSize);
   pQAttr->iFileSize2 = ntohl(pQAttr->iFileSize2);
   plFileSizeC = (unsigned long *) &(pQAttr->iFileSize);
   lFileSize = *plFileSizeC;

   /* reconvert to net format */
   pQAttr->iFileSize = htonl(pQAttr->iFileSize);
   pQAttr->iFileSize2 = htonl(pQAttr->iFileSize2);

   if (lFileSize) 
   {
      if ( (pQAttr->iFileSize2 == 0) ||
           (sizeof(long) == 8) )
         sprintf(cMsg1, "  %12lu", lFileSize);
      else
         sprintf(cMsg1, "    %s   ", cTooBig); 
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
      {
         sprintf(cMsg1,
            "    staged on data mover %s, FS %d (poolId %d)\n",
            pQAttr->cNode, ntohl(pQAttr->iFS),
            ntohl(pQAttr->iPoolId));
         strcat(cMsg, cMsg1);
      }

      sprintf(cMsg1,
         "    obj-Id: %u-%u, restore order: %u-%u-%u-%u-%u\n",
         ntohl(pQAttr->iObjHigh),
         ntohl(pQAttr->iObjLow),
         ntohl(pQAttr->iRestoHigh),
         ntohl(pQAttr->iRestoHighHigh),
         ntohl(pQAttr->iRestoHighLow),
         ntohl(pQAttr->iRestoLowHigh),
         ntohl(pQAttr->iRestoLow) );
      strcat(cMsg, cMsg1);

      sprintf(cMsg1,
         "    version %d, owner: %s, OS: %s, mgmt-class: %s, file set %d\n",
         ntohl(pQAttr->iVersion), pQAttr->cOwner,
         pQAttr->cOS, pQAttr->cMgmtClass, ntohl(pQAttr->iFileSet));
      strcat(cMsg, cMsg1);
   }

   iMsg = strlen(cMsg);

   if (iOut < iMsg)
   {
      fprintf(fLogFile,
         "-W- %s: output string provided too short (%d byte), %d byte needed\n",
        cModule, iOut, iMsg);
      strncpy(pcOut, cMsg, (unsigned) iOut-1);
      strcat(pcOut, "\0");
      
      fprintf(fLogFile,
         "-W- %s: query information incomplete:\n%s\n", cModule, pcOut);

      iMsg = iOut;
   }
   else
   {
      if (iDebug) fprintf(fLogFile,
         "    %s: length query info %d byte, length of string provided %d byte, msg:\n",
         cModule, iMsg, iOut);
      strcpy(pcOut, cMsg);
   }

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return iMsg;

} /* rawQueryString */

/**********************************************************************
 * rawScanObjbuf:
 *   check if obj already available in obj buffer chain
 * created 10. 7.2003 by Horst Goeringer
 **********************************************************************
 */

int rawScanObjbuf(char *pcPath, char *pcFile,
                  int iObjnoAll, int *piObjBuf0)
{
   char cModule[32] = "rawScanObjbuf";
   int iDebug = 0;

   int ii;
   int iFound = 0;   /* =1: specified file already available in list */
   int iObjnoBuf;                /* no. of objects in current buffer */
   int iObjnoCur = 0;         /* current object no. (in all buffers) */
   int iBufno = 0;                          /* no. of object buffers */
   int *piObjBuf;                    /* ptr to current object buffer */
   int **ppiNextBuf;

   srawRetrList *psRetrList;     /* points to object in buffer chain */

   if (iDebug)
      printf("\n-D- begin %s: check %d objs for %s%s\n",
         cModule, iObjnoAll, pcPath, pcFile);

   piObjBuf = piObjBuf0;

   /* loop over object buffers */
   for (;;)
   {
      iObjnoBuf = *piObjBuf;
      psRetrList = (srawRetrList *) &(piObjBuf[1]);
      iBufno++;
      if (iDebug == 2)
      {
         printf("DDD buffer %d: piObjBuf %p, value %d, psRetrList %p\n",
            iBufno, piObjBuf, *piObjBuf, psRetrList);
         fflush(stdout);
      }

      /* loop over objects in current buffer */
      for (ii=1; ii<=iObjnoBuf; ii++)
      {
         iObjnoCur++;
         if (iDebug == 2)
         {
            printf("    %d: %s%s, addr %p\n",
               iObjnoCur, psRetrList->cNamehl, psRetrList->cNamell,
               psRetrList);
	    fflush(stdout);
         }

         if ( (strcmp(pcFile, psRetrList->cNamell) == 0) &&
              (strcmp(pcPath, psRetrList->cNamehl) == 0) )
         {
            if (iDebug) printf(
               "    file %s%s already available in list\n",
               pcPath, pcFile);
            iFound = 1;
            goto gEndScan;
         }

         psRetrList++;

      } /* loop over objects in current buffer */

      if (iObjnoCur == iObjnoAll)
      {
         if (iDebug) printf(
            "    all %d files scanned\n", iObjnoAll);
         break;
      }

      if (psRetrList)
      {
         ppiNextBuf = (int **) psRetrList;
         piObjBuf = *ppiNextBuf;
         psRetrList = (srawRetrList *) &(piObjBuf[1]);
         if (iDebug == 2)
	 {
	    printf("DDD next piObjBuf %p, value %d, psRetrList %p\n",
               piObjBuf, *piObjBuf, psRetrList);
	    fflush(stdout);
	 }
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

/**********************************************************************
 * rawSortValues:
 *   sort indexed list of numbers
 * created 15. 6.2000 by Horst Goeringer
 **********************************************************************
 */

int rawSortValues(
      int *iaValue,
      int iAll,
      int iFirst,
      int iLast,
      int *iaIndex,
      int *iaIndNew)
{
   char cModule[32] = "rawSortValues";
   int iDebug = 0;

   int ii, ii1, iif, jj;
   int iLoop;
   int iInd;
   int iFound = 0;
   int iaValNew[MAX_FILE_NO];

   iLoop = iLast - iFirst + 1;

   if (iDebug)
      printf("\n-D- begin %s\n", cModule);

   if (iDebug)
   {
      printf("    numbers on input:\n                    value  index\n");
      for (ii=iFirst; ii<=iLast; ii++)
      {
         printf("       %6d: %10d %6d\n", ii, iaValue[ii], iaIndex[ii]);
      }
   }

   iFound = iFirst - 1;                /* 1st possible index: iFirst */
   for (jj=iFirst; jj<=iLast; jj++)
   {
      if (iFound >= iFirst)
      {
         for (iif=iFirst; iif<=iFound; iif++)
         {
            if ( (iaValue[jj] < iaValNew[iif]) && (iaValNew[iif]) )
            {
               for (ii1=iFound; ii1>=iif; ii1--)
               {
                  iaValNew[ii1+1] = iaValNew[ii1];
                  iaIndNew[ii1+1] = iaIndNew[ii1];
               }

               iaValNew[iif] = iaValue[jj];
               iaIndNew[iif] = iaIndex[jj];
               iFound++;
               if (iDebug == 2) fprintf(fLogFile,
                  "    at pos %d inserted: val %d, ind %d\n",
                  iif, iaValNew[iif], iaIndNew[iif]);

               break;
            }
            else
            {
               if (iif == iFound)
               {
                  iFound++;
                  iaValNew[iFound] = iaValue[jj];
                  iaIndNew[iFound] = iaIndex[jj];
                  if (iDebug == 2) fprintf(fLogFile,
                     "    at pos %d appended: val %d, ind %d\n",
                     iFound, iaValNew[iFound], iaIndNew[iFound]);

                  break;                                 /* loop iif */
               }

               continue;                                 /* loop iif */
            }
         } /* loop iif */
      } /* (iFound) */
      else
      {
         iaValNew[iFirst] = iaValue[jj];
         iaIndNew[iFirst] = iaIndex[jj];
         iFound = iFirst;

         if (iDebug == 2) fprintf(fLogFile,
            "    start value: val %d, ind %d\n",
            iaValNew[iFound], iaIndNew[iFound]);
      }
   } /* loop objects */

   memcpy(&iaValue[iFirst], &iaValNew[iFirst], (unsigned) iLoop*iint);

   if (iDebug)
   {
      printf("    numbers on output:\n                    value  index\n");
      for (ii=iFirst; ii<=iLast; ii++)
      {
         printf("       %6d: %10d %6d\n", ii, iaValue[ii], iaIndNew[ii]);
      }
   }

   if (iDebug)
      printf("-D- end %s\n\n", cModule);

   return 0;

} /* end rawSortValues*/

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
   char cModule[32] = "ielpst";
   int iDebug = 0;

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
