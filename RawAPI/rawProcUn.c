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
 * created 16. 5.1997 by Horst Goeringer
 *********************************************************************
 * rawProcUn.c
 *    utility programs for mass storage program package:
 *    Unix specific functions
 *    new version (client and server)
 *********************************************************************
 * rawGetDirEntries: get list of entries in FS (via opendir/readdir)
 * rawGetFSEntries:  get list of entries in FS (via scandir)
 * rawGetFSfree:     get free space in specified filesystem (via ls)
 * rawGetFSSpace:    get space statistics of file system (via statfs)
 * rawGetFileAttr:   get file attributes
 * rawGetFileList:   get/enhance file list from generic input
 * rawGetHostConn:   get network connection type of client host
 * rawGetUserid:     get user identification
 *********************************************************************
 *  4. 2.1998, H.G.: new entry rawGetFSfree
 *  6. 2.1998, H.G.: rawGetFSfree: ex shell-cmd via system()
 * 13. 4.1999, H.G.: mod. declaration of rawGetFileAttr
 * 22. 2.2000, H.G.: rawGetFileAttr: fix occurence of pipe message:
 *                   '\nYour .kshrc is not executable!'
 * 28. 7.2000, H.G.: rawGetFileList: ls -pdL -> ls
 * 18. 6.2001, H.G.: rawGetFileList: check upper case in names
 * 21. 6.2001, H.G.: rawGetFileList: control acceptance of upper case
 * 31.10.2001, H.G.: rawGetFSfree added
 * 26. 2.2002, H.G.: rawGetFSSpace added
 *  6. 6.2002, H.G.: rawGetDirEntries, rawGetFSEntries added
 * 29.10.2002, H.G.: rawGetFileList: MAX_FILE_NO -> MAX_STAGE_FILE_NO
 * 31. 1.2003, H.G.: use rawdefn.h
 * 25. 6.2003, H.G.: handle offset of old files in filelist
 *                   rename rawGetFilelist -> rawGetFileList
 *  9. 7.2003, H.G.: rawGetFileList: ignore directories
 * 16. 7.2003, H.G.: rawGetFileList: avoid duplicate file names
 *********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock.h>
#include <sys\types.h>
#define popen _popen
#define pclose _pclose
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/statfs.h>
#include <sys/dir.h>
#endif

#include "rawcommn.h"
#include "rawclin.h"
#include "rawdefn.h"

extern FILE *fLogFile;

#define BUFSIZE_SMALL 80

/********************************************************************
 * rawGetDirEntries:
 *    get list of entries in file system (via opendir/readdir)
 *
 * created  6. 6.2002, Horst Goeringer
 ********************************************************************/

#ifndef WIN32

int rawGetDirEntries(char *pcStageFS)
{
   int iDebug = 0;
   char cModule[32]="rawGetDirEntries";
   int iRC;
   int iEntries = 0;

   DIR *pDir;
   struct dirent *pEntry;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: FS %s\n", cModule, pcStageFS);

   pDir = opendir(pcStageFS);
   if (pDir == NULL)
   {
      fprintf(fLogFile,
         "-E- cannot open directory %s\n", pcStageFS);
      perror("    ");
      return -1;
   }

   while ( (pEntry = readdir(pDir)) != NULL)
   {
      if ( (strcmp(pEntry->d_name, ".") == 0) ||
           (strcmp(pEntry->d_name, "..") == 0) )
         continue;

      iEntries++;

      if (iDebug)
         fprintf(fLogFile, "    %d: %s\n", iEntries, pEntry->d_name);
   }

   iRC = closedir(pDir);

   if (iDebug)
      fprintf(fLogFile, "    rc(closedir) = %d\n-D- end %s\n\n", iRC, cModule);

   return(iEntries);

} /* rawGetDirEntries*/

/********************************************************************
 * rawGetFSEntries:
 *    get list of entries in file system (via scandir)
 *
 * created  6. 6.2002, Horst Goeringer
 ********************************************************************/

int rawGetFSEntries(char *pcStageFS)
                 /* struct dirent *(fEntryList[]) */
{
   int iDebug = 0;
   char cModule[32]="rawGetFSEntries";
   int iRC;
   int ii = 0;

   int iEntries = 0;
   struct dirent **fEntryList;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: FS %s\n", cModule, pcStageFS);

   iRC = scandir(pcStageFS, &fEntryList, NULL, NULL);
   if (iRC < 0 )
   {
      fprintf(fLogFile,
         "-E- calling system function scandir, rc=%d\n", iRC);
      perror("    ");
      return -1;
   }

   iEntries = 0;
   for (ii=0; ii<iRC; ii++)
   {
      if (iDebug) fprintf(fLogFile,
         "    %d: %s\n", ii, fEntryList[ii]->d_name);

      if ( (strcmp(fEntryList[ii]->d_name, ".") == 0) ||
           (strcmp(fEntryList[ii]->d_name, "..") == 0) )
         continue;

      iEntries++;
   }

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return(iEntries);

} /* rawGetFSEntries*/


/********************************************************************
 * rawGetFSfree: get free space (bytes) in specified filesystem
 *
 * created 4.2.98, Horst Goeringer
 ********************************************************************/

int rawGetFSfree(char *pcStageFS)
{
   int iDebug = 0;
   char cModule[32]="rawGetFSfree";
   int iRC;
   unsigned int itemsize, itemno;
   int iFree, *piFree;
   int iSleep = 0;

   FILE *fPipe;
   int iBuf;
   char cName[BUFSIZE_SMALL] = "", *pName;
   char cBuf[BUFSIZE_SMALL] = "", *pBuf;
   char cTempFile[128] = "";
   char cCmd[256] = "/home/rawserv/rawdf.sh ", *pCmd;

   time_t tTime;
   pid_t pstr;

   pBuf = &cBuf[0];
   pName = &cName[0];
   strcpy(cTempFile, pcStageFS);

   tTime = time(NULL);
   pstr = getpid();
   sprintf(cName, "/size.t%d.p%d", tTime, pstr);
   strcat(cTempFile, pName);

   strcat(cCmd, pcStageFS);
   strcat(cCmd, " ");
   strcat(cCmd, cTempFile);

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s: execute\n    '%s'\n",
              cModule, cCmd);

   if (system(NULL))
   {
      iRC = system(cCmd);
      if (iRC)
      {
         fprintf(fLogFile, "-W- %s: system() failed, rc = %d\n",
                 cModule, iRC);
         return -1;
      }
      else
      {
         if (iDebug)
            fprintf(fLogFile,
                    "    shell command successfully executed\n");
      }
   }
   else
   {
      fprintf(fLogFile, "-W- %s: system() not available\n",
              cModule);
      return -1;
   }

   fPipe = NULL;
   fPipe = fopen(cTempFile, "r");
   if (fPipe == NULL)
   {
      fprintf(fLogFile, "-W- %s: opening file %s\n",
              cModule, cTempFile);
      perror("    ");
      return -1;
   }
   if (iDebug)
      fprintf(fLogFile, "    file %s opened\n", cTempFile);

   itemsize = sizeof(char);
   itemno = BUFSIZE_SMALL;
   iBuf = fread(pBuf, itemsize, itemno, fPipe);
   if (iBuf <= 0)
   {
      fprintf(fLogFile, "-W- %s: fread, rc = %d\n", cModule, iBuf);
      perror("    ");
      fprintf(fLogFile, "    NO status buffer sent\n");
   } /* (iBuf <= 0) */
   else
   {
      iFree = atoi(pBuf);
      if (iDebug)
      {
         fprintf(fLogFile, "    %d bytes in file: %s", iBuf, pBuf);
         fprintf(fLogFile, "    free kB: %d\n", iFree);
      }
   }

   iRC = fclose(fPipe);
   if (iRC)
   {
      fprintf(fLogFile, "-W- %s: rc = %d closing file\n",
              cModule, iRC);
      perror("    ");
      return(-1);
   }
   if (iDebug)
      fprintf(fLogFile, "    file %s closed\n", cTempFile);

   iRC = remove(cTempFile);
   if (iRC)
   {
      fprintf(fLogFile, "-W- %s: rc = %d removing file %s\n",
              cModule, iRC, cTempFile);
      perror("    ");
      return -1;
   }
   if (iDebug)
      fprintf(fLogFile, "    file %s removed\n", cTempFile);

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   if (iBuf <= 0) return -1;
   else return(iFree);

} /* rawGetFSfree */

/**********************************************************************
 * rawGetFSSpace
 *    get file size (bytes)
 * created 19.2.2002, Horst Goeringer
 **********************************************************************
 */

int rawGetFSSpace(char *pcFileSystem,
                   int *piSizeAll,
                   int *piSizeAvail,
                   int *piNodeAll,
                   int *piNodeAvail)
{
   char cModule[32] = "rawGetFSSpace";
   int iDebug = 0;

   int iRC;
   int iBlockSize = 4;       /* kB */
   int iSizeAll = 0;
   int iSizeFree = 0;
   int iSizeAvail = 0;
   int iNodeAll = 0;
   int iNodeAvail = 0;

   struct statfs sFSstatus, *pFSstatus;

   if (iDebug) fprintf(fLogFile,
      "\n-D- begin %s: FS %s\n", cModule, pcFileSystem);

   pFSstatus = &sFSstatus;

   iRC = statfs(pcFileSystem, pFSstatus);
   if (iRC)
   {
      fprintf(fLogFile,
         "-E- calling system function statfs, rc=%d\n", iRC);
      perror("    ");
      return -1;
   }

   iBlockSize = (int) pFSstatus->f_bsize;
   if (iBlockSize == 0)
   {
      fprintf(fLogFile,
         "-W- unexpected blocksize found: 0 byte\n");
      return 1;
   }
   else
   {
      if (iDebug) fprintf(fLogFile,
         "    blocksize found %d byte\n", iBlockSize);
      if (iBlockSize >= 1024)
         iBlockSize /= 1024;
      else
      {
         fprintf(fLogFile,
            "-W- unexpected blocksize found: %d byte\n", iBlockSize);
         return 1;
      }
   }

   /* convert to MByte */
   iSizeAll = (int) (pFSstatus->f_blocks/1000)*iBlockSize;
   iSizeFree = (int) (pFSstatus->f_bfree/1000)*iBlockSize;
   iSizeAvail = (int) (pFSstatus->f_bavail/1000)*iBlockSize;

   iNodeAll = (int) pFSstatus->f_files;
   iNodeAvail = (int) pFSstatus->f_ffree;

   if (iDebug)
   {
      fprintf(fLogFile,
         "    overall space %d MB, free %d MB, available %d MB\n",
         iSizeAll, iSizeFree, iSizeAvail);
      fprintf(fLogFile,
         "    overall file nodes %d, available %d\n",
         iNodeAll, iNodeAvail);
   }

   *piSizeAll = iSizeAll;
   *piSizeAvail = iSizeAvail;
   *piNodeAll = iNodeAll;
   *piNodeAvail = iNodeAvail;

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   return 0;

} /* rawGetFSSpace */

#endif

/**********************************************************************/
/* rawGetFileAttr:  get file attributes (size in bytes)               */
/*                  utilize shell command output via pipes            */
/* created 17.4.96, Horst Goeringer                                   */
/**********************************************************************/

int rawGetFileAttr(char *pcFile,
                   unsigned long *piFileSize)
{
   int iDebug = 0;
   char cModule[32] = "rawGetFileAttr";
   FILE *f_ifile, *f_ofile;
   int iRC;
   unsigned long iFileSize, iiu;
   int iReclen;

   int ilocSize = 5;     /* token no. 5 contains file size */
   int ilocFile = 9;     /* token no. 9 contains file name (check) */

   int ii, ilen, iloc;
   unsigned long lr;

   char cCmd[CMDLEN] = "ls -l ";
   char *pCmd;
   char *pType = "r";
   char *pBuf, *pBuf0;
   char *pcDollar = "$";  /* requires special treatment in file name */
   char *pc, *ploc;

   char *pctoken, *pcblank = " ";
   char ctoken[20];
   int icount = 0;

/******************** end of declarations *****************/

   if (iDebug)
      fprintf(fLogFile, "-D- Begin %s\n", cModule);

   iReclen = 0;            /* not available in Unix system */

   /* treat $ in file name */
   ploc = strchr(pcFile, *pcDollar);
   if (ploc != NULL)
   {
      if (iDebug)
         fprintf(fLogFile, "-D- $ in file name found: %s\n", pcFile);
      ilen = strlen(pcFile);
      pc = pcFile;
      for (ii = 1; ii <= ilen; ii++)
      {
         iloc = strncmp(pc, pcDollar, 1);
         if (iloc == 0) strncat(cCmd, "\\", 1);        /* $ found */
         strncat(cCmd, pc++, 1);
      }
   } /* ploc != NULL, $ in file name */
   else strcat(cCmd, pcFile);

   if (iDebug)
      fprintf(fLogFile, "-D- %s: ex '%s'\n", cModule, cCmd);

   pCmd = &cCmd[0];
   f_ifile = popen(pCmd, pType);
   if (f_ifile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening pipe\n", cModule);
      return(-1);
   }
   if (iDebug)
      fprintf(fLogFile, "    %s: pipe opened\n", cModule);

   if ( !(pBuf0 = (char *) malloc(BUFSIZE)) )
   {
      fprintf(fLogFile,
              "-E- %s: allocation buffer failed\n", cModule);
      pclose(f_ifile);
      return(-1);
   }
   if (iDebug)
      fprintf(fLogFile, "    %s: buffer allocated\n", cModule);

   pBuf = pBuf0;
   lr = fread(pBuf, sizeof(char), BUFSIZE, f_ifile);
   if (lr <= 0)
   {
      fprintf(fLogFile,
              "-E- %s: reading from pipe failed\n", cModule);
      pclose(f_ifile);
      return(-1);
   }

   pBuf += (lr-1);
   strncpy(pBuf, "\0", 1);    /* overwrite newline character */

   if (iDebug)
      fprintf(fLogFile, "-D- %s: complete string: (%d bytes): \n%s\n",
              cModule, lr, pBuf0);

   pBuf = pBuf0;
   while ( (pctoken = strtok(pBuf, pcblank)) != NULL)
   {
      icount++;
      if (iDebug)
         fprintf(fLogFile, "    token %d: %s\n", icount, pctoken);

      if (icount == 2)
      {
         /* check for leading string:
            '\nYour .kshrc is not executable!' */
         iRC = strcmp(pctoken, ".kshrc");
         if (iRC == 0)
         {
            fprintf(fLogFile, "    %s: token %d: %s found\n",
                    cModule, icount, pctoken);
            ilocSize += 4;
            ilocFile += 4;
         }
      }

      if (icount == ilocSize)
      {
         /* strcpy(ctoken, "2000000000"); test large file size */
         if ( ( iRC = sscanf( pctoken, "%u", &iFileSize) ) <= 0 )
         {
            fprintf(fLogFile,
                    "-E- %s: file size %d (%s) invalid\n",
                    cModule, iFileSize, pctoken);
            perror("    ");
            pclose(f_ifile);
            return(-1);
         }
         if (iDebug)
            fprintf(fLogFile, "    file size %u\n", iFileSize);

         if ( (iFileSize == 0) && (iDebug) )
            fprintf(fLogFile, "    file %s empty\n", pcFile);

         if ( (iFileSize > MAX_FILE_SIZE) ||
              (iFileSize < 0) )
         {
            fprintf(fLogFile, "-E- file %s too large\n", pcFile);
            if (iFileSize > 0)
               fprintf(fLogFile,
                       "    file size %u > 2 GBbyte - 1\n", iFileSize);
            else
               fprintf(fLogFile, "    file size > 2 GBbyte - 1\n");
            pclose(f_ifile);
            return(-1);
         }
      }

      if (icount == ilocFile)
      {
         if ( (iRC = strcmp(pctoken, pcFile) ) != 0)
         {
           fprintf(fLogFile,
                  "-E- %s: file name check: %s\n", cModule, pctoken);
           pclose(f_ifile);
           return(-1);
         }
      }

      pBuf = NULL;

   } /* while ... */

   pclose(f_ifile);
   *piFileSize = iFileSize;
   if (iDebug)
      fprintf(fLogFile, "-D- End %s\n", cModule);

   return 0;

} /* rawGetFileAttr */

/**********************************************************************/
/* rawGetFileList: execute shell cmd ls and get file list vector */
/* created 19.2.96, Horst Goeringer */
/**********************************************************************/

int rawGetFileList( char *pcFile,
                    int iAccept,      /* = 0: only lower case files
                                         = 1: also upper case files
                                         = 2: also directories       */
                    int iEntryLoop,
                    char **pFilelist)
{
   char cModule[32] = "rawGetFileList";
   int iDebug = 0;
   int iRC = 0;
   int iRCE = 0;
   int iIgnore = 0;                  /* =1: new file already in list */
   int ilen, iloc, ii, jj;
   int ird, ipc, ipc1, irem, grem = 0;
   int iSize;
   int iFileno, iFilenoo;
   int *piFileno;              /* points to no. of files in filelist */
   int *piFilelist;              /* points to first file in filelist */

   char *pc, *pc0, *ploc;
   unsigned long lr;

   FILE *f_ifile;

   char cCmd[CMDLEN] = "ls -L ";
                         /* mark directory names with / to skip them */
   /* char cCmd[CMDLEN] = "ls ";  was valid from 28.7.2000-18.6.2001 */
   char *pCmd;
   char *pBuf;
   char cBuf[MAX_FILE];     /* temp buffer for incomplete file names */
   char *pcDollar = "$";  /* requires special treatment in file name */

   srawArchList *psArchList,    /* points to actual file in filelist */
                *psArchList0,    /* points to first file in filelist */
                *psArchList0c;       /* current file in old filelist */

   /******************************************************************/

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   ploc = strchr(pcFile, *pcDollar);
   if (ploc != NULL)
   {
      if (iDebug)
         fprintf(fLogFile, "    '$' in file name found: %s\n", pcFile);
      ilen = strlen(pcFile);
      pc = pcFile;
      for (ii=1; ii<=ilen; ii++)
      {
         iloc = strncmp(pc, pcDollar, 1);
         if (iloc == 0) strncat(cCmd, "\\", 1);           /* $ found */
         strncat(cCmd, pc++, 1);
      }
   } /* ploc != NULL, $ in file name */
   else strcat(cCmd, pcFile);
   strcat(cCmd, " 2>/dev/null");

   if (iDebug)
      fprintf(fLogFile, "    command: %s\n", cCmd);

   piFilelist = (int *) *pFilelist;/* points now to file list buffer */
   piFileno = piFilelist++; /* points to no. of files, updated later */
   iFilenoo = *piFileno;
   psArchList = (srawArchList *) piFilelist;
   psArchList0 = psArchList;        /* points now to first file name */

   if (iFilenoo)                   /* skip files from previous calls */
   {
      if (iDebug) fprintf(fLogFile,
         "    %d old files in list\n", iFilenoo);

      for (ii=1; ii<=iFilenoo; ii++)
      {
         if (iDebug == 1) fprintf(fLogFile,
            "    %d (old): %s\n", ii, psArchList->cFile);
         psArchList++;
      }
   }

   pCmd = &cCmd[0];
   f_ifile = popen(pCmd, "r");
   if (f_ifile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening pipe\n", cModule);
      return(-1);
   }
   if (iDebug) fprintf(fLogFile, "    pipe opened\n");

   if ( !(pBuf = (char *) malloc(BUFSIZE)) )
   {
      fprintf(fLogFile,
              "-E- %s: allocation buffer failed\n", cModule);
      pclose(f_ifile);
      return(-1);
   }
   if (iDebug) fprintf(fLogFile, "    buffer allocated\n");

   memset(&cBuf, '\0', MAX_FILE);
   lr = 1;
   iFileno = 0;
   while (lr > 0)
   {
gRead:
      lr = fread(pBuf, sizeof(char), BUFSIZE, f_ifile);
      if (lr > 0)
      {
         ird = lr;        /* meaningful bytes     */
         pc0 = pBuf;
         pc = pBuf;
         pc += ird;
         *(pc) = '\0';        /* overwrite first trailing blank */

         if (iDebug == 2)
            fprintf(fLogFile, "-D- received %d bytes:\n", ird);

         pc = pBuf;
         while(ird > 0)
         {
            if (iDebug == 2)
               fprintf(fLogFile, "    '%s'", pc0);
            ipc = strcspn(pc0, "\n");
            irem = strlen(pc0);
            pc = strchr(pc0, '\n');
            if (iDebug == 2)
               fprintf(fLogFile, "    first length %d, total %d\n",
                      ipc, irem);

            if (grem)   /* incompl. file name from previous buffer */
            {
               if (ipc > 0)
               {
                  strncat(cBuf, pc0, ipc);
                  if (iDebug == 2)
                     fprintf(fLogFile, "    last  concatenated: %s\n",
                     cBuf);
               }

               ii = strlen(cBuf);
               if ( (strcmp(cBuf, "./")) && (ii) &&
                    (strncmp(&(cBuf[ii-1]), ":", 1)) )
               {
                  iRC = rawTestFileName(cBuf);
                  if ( (iRC == 0) ||
                       ( (iRC == 1) && (iAccept >= 1) ) )
                  {
                     if (iFilenoo)
                     {
                        psArchList0c = psArchList0; /* first old file */

                        /* compare new name with old ones */
                        for (jj=1; jj<=iFilenoo; jj++)
                        {
                           iRC = strcmp(cBuf, psArchList0c->cFile);
                           if (iRC == 0)
                           {
                              iIgnore = 1;
                              if (iDebug) fprintf(fLogFile,
                                 "    entry %s already available(1)\n",
                                 cBuf);
                              break;
                           }
                           psArchList0c++;
                        }
                     } /* (iFilenoo) */

                     if (iIgnore == 0)
                     {
                        strcpy(psArchList->cFile, cBuf);
                        iFileno++;
                        if (iDebug == 1) fprintf(fLogFile,
                           "    %s stored(1), addr %d\n",
                           psArchList->cFile, psArchList);

                        psArchList++;
                     }
                     else
                        iIgnore = 0;

                     if (iFileno >= MAX_STAGE_FILE_NO)
                     {
                        fprintf(fLogFile,
                           "-E- %s: List of files for archive/retrieve currently limited to %d entries\n",
                           cModule, --iFileno);
                        fprintf(fLogFile,
                                "    %s: NOT ALL files handled\n",
                                cModule);
                        goto gFinishList;
                     }
                  }
                  else
                  {
                     if (iRC == 1)
                     {
                        fprintf(fLogFile,
                           "-W- file name %s has uppercase letters - ignored\n",
                           cBuf);
                        iRCE = 1;     /* client ends with error code */
                     }
                     if (iRC == 2)                  /* no error code */
                     {
                        fprintf(fLogFile,
                                "-W- directory %s ignored\n", cBuf);
                     }
                  }
               }

               memset(&cBuf, '\0', strlen(cBuf));
               grem = 0;
            } /* if (grem) */
            else
            {
               strncpy(cBuf, pc0, ipc);
               strcat(cBuf, "\0");
               if (irem - ipc == 0) /* current file name incomplete */
               {
                  ipc1 = strlen(cBuf);
                  if (iDebug == 2) fprintf(fLogFile,
                     "    grem set, cBuf: %s (%d byte) \n", cBuf, ipc1);
                  grem = 1;
               }
               else
               {
                  ii = strlen(cBuf);
                  if ( (strcmp(cBuf, "./")) && (ii) &&
                       (strncmp(&(cBuf[ii-1]), ":", 1)) )
                  {
                     iRC = rawTestFileName(cBuf);
                     if ( (iRC == 0) ||
                          ( (iRC == 1) && (iAccept >= 1) ) )
                     {
                        if (iFilenoo)
                        {
                           psArchList0c = psArchList0;/* 1st old file */

                           /* compare new name with old ones */
                           for (jj=1; jj<=iFilenoo; jj++)
                           {
                              if (ipc == strlen(psArchList0c->cFile))
                              {
                                 iRC = strncmp(cBuf,
                                               psArchList0c->cFile,
                                               ipc);
                                 if (iRC == 0)
                                 {
                                    iIgnore = 1;
                                    if (iDebug) fprintf(fLogFile,
                                       "    entry %s already available(2)\n",
                                       cBuf);
                                    break;
                                 }
                              }
                              psArchList0c++;
                           }
                        } /* (iFilenoo) */

                        if (iIgnore == 0)
                        {
                           strncpy(psArchList->cFile, cBuf, ipc);
                           iFileno++;

                           if (iDebug == 1) fprintf(fLogFile,
                              "    %s stored, addr %d\n",
                              psArchList->cFile, psArchList);
                           psArchList++;
                        }
                        else
                           iIgnore = 0;

                        if (iFileno >= MAX_STAGE_FILE_NO)
                        {
                           fprintf(fLogFile,
                              "-E- %s: List of files for archive/retrieve currently limited to %d entries\n",
                              cModule, --iFileno);
                           fprintf(fLogFile,
                                   "    %s: NOT ALL files handled\n",
                                   cModule);
                           goto gFinishList;
                        }
                     }
                     else
                     {
                        if (iRC == 1)
                        {
                           fprintf(fLogFile,
                              "-W- file name %s has uppercase letters - ignored\n",
                              cBuf);
                           iRCE = 1;  /* client ends with error code */
                        }
                        if (iRC == 2)               /* no error code */
                           fprintf(fLogFile,
                                   "-W- directory %s ignored\n", cBuf);
                     }
                  }

                  memset(&cBuf, '\0', strlen(cBuf));
               }

            } /* if (!grem) */

            pc0 = ++pc;
            ird -= (ipc+1);
         } /* while(ird > 0) */

      } /* if(lr > 0) */

   } /* while(lr > 0) */

gFinishList:
   if (iEntryLoop)
   {
      if (iFileno == 0)
         fprintf(fLogFile, "-I- no (new) files found\n");
      else
         fprintf(fLogFile, "-I- %d (new) files found\n", iFileno);
   }

   if ( (iDebug) && (iFileno) )
   {
      psArchList = (srawArchList *) piFilelist;
                                    /* points now to first file name */
      for (ii=1; ii<=iFilenoo; ii++)
      {
         if (iDebug == 1) fprintf(fLogFile,
            "    %d (old): %s\n", ii, psArchList->cFile); psArchList++;
      }
      for (ii=iFilenoo+1; ii<=iFilenoo+iFileno; ii++)
      {
         if (iDebug == 1) fprintf(fLogFile,
            "    %d: %s\n", ii, psArchList->cFile); psArchList++;
      }
   }

   iFileno += iFilenoo;                  /* total no. of files found */
   piFileno[0] = iFileno;           /* pass total no. of files found */

   iRC = pclose(f_ifile);
   if (iDebug)
   {
      if (iRC)
      {
         fprintf(fLogFile, "-E- %s: iRC = %d closing pipe\n",
                 cModule, iRC);
         perror("    ");
      }
      else
         fprintf(fLogFile, "    pipe closed\n", iRC);

      fprintf(fLogFile, "-D- end %s\n\n", cModule);
   }

   if (iRCE) return iRCE;
   else return 0;

} /* rawGetFileList */

/**********************************************************************
 * rawGetHostConn:   get network connection type of client host
 * created 19.3.98, Horst Goeringer
 **********************************************************************/

int rawGetHostConn()
{
   int iDebug = 0;
   char cModule[32] = "rawGetHostConn";
   int iRC;
   int ii;
   int iBuf = 0;
   int iCol = 0;                    /* no. of columns in cmd output */
   int iType = 0;                /* network connection type
                                    = 1: ethernet (slow)
                                    = 2: ethernet (fast, nodes linux*
                                    = 3: fddi
                                    = 4: SP switch */
   char *pc;
   char cToken[16] = "", *pToken;
   char cheadName[16], *pheadName;
   char cheadMtu[16] = "mtu",  *pheadMtu;
   char cName[16] = "", *pName;
   char cMtu[16] = "",  *pMtu;
   char cNameRef1[16] = "";
   char cNameRef2[16] = "";
   char cMtuRef1[16] = "";

   char cCmd[CMDLEN] = "netstat -i", *pCmd;
   char cBuf[1024] = "", *pBuf;
   char cNode[MAX_NODE] = "";

   FILE *f_ifile;

   if (iDebug)
      printf("\n-D- in %s\n", cModule);

#ifdef _AIX
   strcpy(cheadName, "name");
#else
   strcpy(cheadName, "iface");
#endif

   iRC = gethostname(cNode, MAX_NODE);
   if (iRC)
   {
      fprintf(fLogFile,
              "-E- %s: getting client host name: %s\n",
              cModule, strerror(iRC));
      return(1);
   }
   if (iDebug)
      printf("    %s: client host %s\n", cModule, cNode);

   if (strncmp(cNode, "lx", 2) == 0)               /* fast ethernet */
      return(2);
   if (strncmp(cNode, "linux", 5) == 0)            /* fast ethernet */
      return(2);
   if (strncmp(cNode, "sp2", 3) == 0)                       /* fddi */
      return(4);

   pToken = &cToken[0];
   pheadName = &cheadName[0];
   pheadMtu = &cheadMtu[0];
   pName = &cName[0];
   pMtu = &cMtu[0];
   pCmd = &cCmd[0];
   pBuf = &cBuf[0];

   f_ifile = popen(pCmd, "r");
   if (f_ifile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening pipe\n", cModule);
      return(-1);
   }
   if (iDebug) printf("    %s: pipe opened\n", cModule);

   iBuf = fread(pBuf, sizeof(char), 1024, f_ifile);
   if (iBuf <= 0)
   {
      fprintf(fLogFile, "-E- %s: reading from pipe failed\n", cModule);
      goto gError;
   }

   if (iDebug == 2)
      printf("    %s command output: \n%s", cModule, cBuf);

   pToken = strtok(pBuf, " \n");
   pc = pToken;
   while (*pc != '\0') { *pc++ = tolower(*pc); }
   if (strcmp(pToken, pheadName))
   {
      fprintf(fLogFile, "-E- %s: invalid name heading (%s, expected %s)\n",
              cModule, pToken, pheadName);
      goto gError;
   }

   pToken = strtok(NULL, " \n");
   pc = pToken;
   while (*pc != '\0') { *pc++ = tolower(*pc); }
   if (strcmp(pToken, pheadMtu))
   {
      fprintf(fLogFile, "-E- %s: invalid mtu heading (%s, expected %s)\n",
              cModule, pToken, pheadMtu);
      goto gError;
   }

#ifdef _AIX
   iCol = 9;
   strcpy(cNameRef1, "en0");
   strcpy(cNameRef2, "fi0");
#endif
#ifdef Linux
   iCol = 12;
   strcpy(cNameRef1, "eth0");
   strcpy(cNameRef2, "fdd0");
#endif

   for (ii=1; ii<=iCol-2; ii++)              /* skip other headings */
   { pToken = strtok(NULL, " \n"); }

   for (;;)                       /* loop over command output words */
   {
      pToken = strtok(NULL, " \n");
      if (pToken == NULL) break;                             /* EOF */
      pc = pToken;
      while (*pc != '\0') { *pc++ = tolower(*pc); }
      if (iDebug == 2)
         printf("DDD %s: %s\n", cModule, pToken);

      if (strcmp(pToken, cNameRef1) == 0)
      {
         iType = 1;
         if (iDebug)
            printf("    %s: ethernet available\n", cModule);
      }
      else if (strcmp(pToken, cNameRef2) == 0)
      {
         iType = 3;                                         /* fddi */
         break;
      }
   } /* loop over command output lines */

   pclose(f_ifile);
   if ( (iType != 3) && (iType != 1) )
   {
      fprintf(fLogFile,
              "-E- %s: invalid network connection type (%d)\n",
              cModule, iType);
      goto gError;
   }
   else if (iDebug)
      printf("-D- end %s: network connection type %d\n\n",
             cModule, iType);
   goto gEnd;

gError:
   iType = -1;

gEnd:
   return(iType);

} /* rawGetHostConn */

/**********************************************************************/
/* rawGetUserid     get user identification */
/* created 22.3.96, Horst Goeringer */
/**********************************************************************/

char *rawGetUserid()
{
   int iDebug = 0;
   char cModule[32] = "rawGetUserid";
   unsigned long lr;
   FILE *f_ifile;

   char cCmd[CMDLEN] = "whoami";
   char *pCmd;
   char *pBuf, *pBuf0;

   pCmd = &cCmd[0];
   f_ifile = popen(pCmd, "r");
   if (f_ifile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening pipe\n", cModule);
      return(NULL);
   }
   if (iDebug) printf("    %s: pipe opened\n", cModule);

   if ( !(pBuf0 = (char *) malloc(BUFSIZE)) )
   {
      fprintf(fLogFile, "-E- %s: allocation buffer failed\n", cModule);
      pclose(f_ifile);
      return(NULL);
   }
   if (iDebug) printf("    %s: buffer allocated\n", cModule);

   pBuf = pBuf0;
   lr = fread(pBuf, sizeof(char), BUFSIZE, f_ifile);
   if (lr <= 0)
   {
      fprintf(fLogFile, "-E- %s: reading from pipe failed\n", cModule);
      pclose(f_ifile);
      return(NULL);
   }

   pBuf += (lr-1);
   strncpy(pBuf, "\0", 1);    /* overwrite newline character */

   pclose(f_ifile);

   if (iDebug)
      printf("-D- %s: user name (%d bytes): %s\n", cModule, lr, pBuf0);

   return(pBuf0);

} /* rawGetUserid */


