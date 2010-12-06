/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 16. 5.1997 by Horst Goeringer
 *********************************************************************
 * rawProcUn.c 
 * Unix specific utility programs for gStore package: client and server
 *********************************************************************
 * rawGetDirEntries: get number of entries in FS (via opendir/readdir)
 * rawGetDirEntryList: get/enhance list of matching FS entries
 * rawGetFSEntries:  get number of entries in FS (via scandir)
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
 *  6. 8.2004, H.G.: ported to Lynx
 *  1. 2.2005, H.G.: ported to Linux and gcc322
 *  2. 2.2007, H.G.: two steps for char ptr increments (gcc 3.3.5)
 *  9. 5.2008, H.G.: remove (client header) file rawclin.h
 * 22. 9.2008, H.G.: rawGetFileList: suppress subdir contents
 * 12.11.2008, H.G.: add suggestions of Hakan
 * 10. 2.2009, H.G.: rawGetFileList: don't stop scan if ls output
 *                   buffer starts with '\n'
 * 11. 2.2009, H.G.: rawGetFileList: stop scanning subdirs 
 *               DDD for recursive filelist: remove stop + ident files
 * 22. 6.2008, H.G.: replace long->int if 64bit client (ifdef SYSTEM64)
 *  3. 9.2009, H.G.: rawGetFileList: MAX_STAGE_FILE_NO -> MAX_FILE_NO
 * 29. 1.2010, H.G.: rawGetFileList, cBuf: MAX_FILE -> MAX_FULL_FILE
 *                   rawGetFSfree, cTempFile: 128 -> MAX_FULL_FILE
 * 24. 6.2010, H.G.: new entry rawGetDirEntryList
 * 12. 8.2010, H.G.: rawGetDirEntryList: increase filelist, if too small
 * 16. 8.2010, H.G.: rawGetDirEntryList: handle (xfs) filesystems
 *                   providing entry type "DT_UNKNOWN"
 * 23. 8.2010, H.G.: rawGetFileAttr, rawGetFileList:
 *                      remove SYSTEM64 (allow "long")
 *                   rawGetUserid: lr: long -> int
 *  5.11.2010, H.G.: replace perror by strerror(errno),
 *                   reset errno after error
 * 26.11.2010, H.G.: rawGetFileAttr, rawGetFileList:
 *                      reset errno after successful!! pclose
 *********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fnmatch.h>

#ifndef Lynx
#include <sys/types.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include <sys/dir.h>
#endif

#include "rawcommn.h"
#include "rawdefn.h"
#include "rawentn.h"

extern FILE *fLogFile;

static int iFileList = sizeof(srawFileList);

#define BUFSIZE_SMALL 80

#ifndef Lynx
/********************************************************************
 * rawGetDirEntries:
 *    get number of entries in file system (via opendir/readdir)
 *
 * created  6. 6.2002, Horst Goeringer
 ********************************************************************/

int rawGetDirEntries(char *pcStageFS)
{
   char cModule[32]="rawGetDirEntries";
   int iDebug = 0;

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
         "-E- %s: cannot open directory %s\n", cModule, pcStageFS);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

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

} /* rawGetDirEntries */
#endif

#ifndef Lynx
/*********************************************************************
 * rawGetDirEntryList:
 *    get/enhance list of matching entries in file system
 * created 23.6.2010, Horst Goeringer
 *********************************************************************
 */

int rawGetDirEntryList(
       char *pcDirName,
       char *pcEntryMask,                         /* entry name mask */
       int iEntryType,
             /* = 0: all entries (in FileList)
                = 1: all files (in FileList)
                = 2: all subdirectories (in DirList)
                =10: all entries, but files only if matching with mask
                =11: all files, but only if matching with mask
                =12: all subdirs, but only if matching with mask     */
       int iEntryLoop,         /* = 1: in loop, append new entries
                                  = 2: recursive, append new entries */
       int iPrefixDir,    /* = 1: prefix files in list with cur path */
       char **ppFileList,                   /* ptr to entry list ptr */
       char **ppDirList)                      /* ptr to dir list ptr */
{
   char cModule[32] = "rawGetDirEntryList";
   int iDebug = 0;                 /* =1: verbose, =2: entry details */
   int iRC = 0;
   int iRCE = 0;
   int ilen, iloc, ii, jj;
   char cTemp[MAX_FULL_FILE] = "";

   int iTypeFound = 0;
      /* keep entry type, if DT_UNKNOWN (xfs): =1: reg file, =2: dir */
   char cEntryType[32] = "";
   int iEntries = 0;      /* no. of entries in specified file system */
   int iEntriesMatch = 0; /* no. of matching entries in specified FS */

   int iFileEntryMax = 0;        /* max. no. of file names in buffer */
   int iFileEntryRem = 0;   /* remaining no. of file names in buffer */
   int iFileEntries = 0;
   int iFileEntriesOld = 0;
   int iFileEntriesNew = 0;
   int *piFileNo;           /* points to no. of entries in file list */
   int *piFileList;                     /* points to first file name */
   srawFileList *psFileList;          /* points to current file name */
   srawFileList *psFileList0;           /* points to first file name */
   
   char *pcFileList2;
   int *piFileList2; /* points to first file in reallocated filelist */

   int iDirEntryMax = 0;       /* max. no. of subdir names in buffer */
   int iDirEntryRem = 0;  /* remaining no. of subdir names in buffer */
   int iDirEntries = 0;
   int iDirEntriesOld = 0;
   int iDirEntriesNew = 0;
   int *piDirNo;            /* points to no. of subdirs in entrylist */
   int *piDirList;                    /* points to first subdir name */
   srawFileList *psDirList;         /* points to current subdir name */
   srawFileList *psDirList0;          /* points to first subdir name */

   int iMatch = 0;    /* = 1: handle only entries matching with mask */
   int iStore = 0;           /* =1: cur entry will be stored in list */
   int iCheck = 0;       /* =1: cur entry must be compared with mask */

   DIR *pDir;
   struct dirent *pEntry;

   struct stat sEntryStatus, *pEntryStatus;

   if (iDebug)
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   pEntryStatus = &sEntryStatus;

   if (strlen(pcDirName) <= 0)
   {
      fprintf(fLogFile, "-E- %s: empty directory name\n", cModule);

      iRCE = -1;
      goto gEndDirEntryList;
   }

   if (iEntryType >= 10)
   {
      if (strlen(pcEntryMask) <= 0)
      {
         fprintf(fLogFile,
            "-E- %s: empty mask for entry name\n", cModule);

         iRCE = -1;
         goto gEndDirEntryList;
      }

      iEntryType -= 10;

      if (strcmp(pcEntryMask, "*") != 0)
         iMatch = 1;
   }
   
   if (iEntryType == 0)
      strcpy(cEntryType, "entries");
   else if (iEntryType == 1)
      strcpy(cEntryType, "files");
   else if (iEntryType == 2)
      strcpy(cEntryType, "subdirs");
   else
   {
      fprintf(fLogFile,
         "-E- %s: invalid EntryType %d (allowed 0-2, 10-12)\n",
         cModule, iEntryType);

      iRCE = -1;
      goto gEndDirEntryList;
   }

   /* handle files */
   if ( (iEntryType == 0) || (iEntryType == 1) )
   {
      if (ppFileList == NULL)
      {
         fprintf(fLogFile,
            "-E- %s: invalid pointer for entry list\n", cModule);

         iRCE = -1;
         goto gEndDirEntryList;
      }

      piFileNo = (int *) *ppFileList;
      piFileList = piFileNo;
      iFileEntriesOld = *piFileNo;

      if (iEntryLoop)
      {
         if (iDebug)
         {
            if (iFileEntriesOld) fprintf(fLogFile,
               "    %d files already available in list:\n",
               iFileEntriesOld);
            else fprintf(fLogFile,
               "    still no files in list\n");
            if (iEntryLoop == 2)
               fprintf(fLogFile, "    recursive file handling\n");
         }
      }
      else
      {
         if (iFileEntriesOld) fprintf(fLogFile,
            "-W- %s: no entry loop, but %d files already available in list\n",
            cModule, iFileEntriesOld);
      }

      piFileList++;
      psFileList = (srawFileList *) piFileList;
      psFileList0 = psFileList;

      if (iDebug)
      {
         if (iFileEntriesOld)
         {
            fprintf(fLogFile, "old files:\n");
            for (ii=1; ii<=iFileEntriesOld; ii++)
            {
               fprintf(fLogFile, " %d: %s\n", ii, psFileList->cFile);
               psFileList++;
            }
         }
      }
      else
         psFileList += iFileEntriesOld;

      iFileEntries = iFileEntriesOld;
      iFileEntryMax = MAX_FILE_NO;
      /* iFileEntryMax = 2;  DDDMax */

      /* already add. space allocated */
      if (iFileEntriesOld > iFileEntryMax)
      {
         ii = iFileEntriesOld / iFileEntryMax;
         ii++;
         iFileEntryMax *= ii; 
      }
      iFileEntryRem = iFileEntryMax - iFileEntriesOld;

      if (iDebug) fprintf(fLogFile,
         "    space available for %d file entries (max %d)\n",
         iFileEntryRem, iFileEntryMax);

   } /* (iEntryType == 0 || 1) */

   /* handle subdirectories */
   if ( (iEntryType == 0) || (iEntryType == 2) )
   {
      if (ppDirList == NULL)
      {
         fprintf(fLogFile,
            "-E- %s: invalid pointer for subdir list\n", cModule);

         iRCE = -1;
         goto gEndDirEntryList;
      }

      piDirNo = (int *) *ppDirList;
      piDirList = piDirNo;
      iDirEntriesOld = *piDirNo;

      if (iEntryLoop)
      {
         if (iDebug)
         {
            if (iDirEntriesOld) fprintf(fLogFile,
               "    %d subdirectories already available in list:\n",
               iDirEntriesOld);
            else fprintf(fLogFile,
               "    still no subdirectories in list\n");
         }
      }
      else if (iDirEntriesOld) fprintf(fLogFile,
         "-W- %s: no entry loop, but %d subdirectories available in list\n",
         cModule, iDirEntriesOld);

      piDirList++;
      psDirList = (srawFileList *) piDirList;
      psDirList0 = psDirList;

      for (ii=1; ii<=iDirEntriesOld; ii++)
      {
         if (iDebug)
         {
            if (ii == 1)
               fprintf(fLogFile, "previous subdirs:\n");
            fprintf(fLogFile, " %d: %s\n", ii, psDirList->cFile);
         }

         psDirList++;
      }

      iDirEntries = iDirEntriesOld;
      iDirEntryMax = MAX_FILE_NO;

      /* already add. space allocated */
      if (iDirEntriesOld > iDirEntryMax)
      {
         ii = iDirEntriesOld / iDirEntryMax;
         ii++;
         iDirEntryMax *= ii; 
      }
      iDirEntryRem = iDirEntryMax - iDirEntriesOld;

      if (iDebug) fprintf(fLogFile,
         "    space available for %d subdir entries (max %d)\n",
         iDirEntryRem, iDirEntryMax);

   } /* (iEntryType == 0 || 2) */

   if (iDebug)
   {
      if ( (iEntryType == 0) && (iMatch) )
      {
         fprintf(fLogFile,
            "    provide all files matching with %s and all subdirs in directory %s\n",
            pcEntryMask, pcDirName);
      }
      else
      {
         fprintf(fLogFile, "    provide all %s", cEntryType);
         if (iMatch) fprintf(fLogFile,
            " matching with %s", pcEntryMask);
         fprintf(fLogFile, " in directory %s\n", pcDirName);
      }
   }

   pDir = opendir(pcDirName);
   if (pDir == NULL)
   {
      fprintf(fLogFile,
         "-E- %s: cannot open directory %s\n", cModule, pcDirName);
      if (errno)
      {
         fprintf(fLogFile, "%s\n", strerror(errno));
         errno = 0;
      }

      iRCE = -1;
      goto gEndDirEntryList;
   }

   while ( (pEntry = readdir(pDir)) != NULL)
   {
      if ( (strcmp(pEntry->d_name, ".") == 0) ||
           (strcmp(pEntry->d_name, "..") == 0) )
         continue;

      iEntries++;
      iCheck = 0;
      iStore = 0;

      /* for xfs */
      if (pEntry->d_type == DT_UNKNOWN)
      {
         if ( (iEntries == 1) && (iDebug) ) fprintf(fLogFile,
            "    %s: of type DT_UNKNOWN (1st entry)\n",
            pEntry->d_name);

         strcpy(cTemp, pcDirName);
         strcat(cTemp, "/");
         strcat(cTemp, pEntry->d_name);

         iRC = stat(cTemp, pEntryStatus);
         if (iRC)
         {
            if (errno)
            {
               fprintf(fLogFile, "-E- stat %s: %s\n",
                  cTemp, strerror(errno));

               /* only valid for 32 bit OS */
               if (strcmp(strerror(errno),
                  "Value too large for defined data type") == 0)
               {
                  fprintf(fLogFile,
                     "-E- %s: filesize of %s > 2 GByte: use 64 bit gStore client\n",
                     cModule, cTemp);

                  errno = 0;
                  return -99;
               }
               errno = 0;
            }
            else fprintf(fLogFile,
               "-W- %s: entry %s unavailable\n", cModule, pEntry->d_name);

            continue;
         }

         if (S_ISREG(pEntryStatus->st_mode))
            iTypeFound = 1;
         else if (S_ISDIR(pEntryStatus->st_mode))
            iTypeFound = 2;
         else
         {
            fprintf(fLogFile,
               "-W- %s: entry %s neither file nor directory, ignored\n",
               cModule, pEntry->d_name);

            continue;
         }
      } /* (pEntry->d_type == DT_UNKNOWN) */
      else
         iTypeFound = 0;

      /* check in which cases entry could be stored */
      if ( (pEntry->d_type == DT_REG) || (iTypeFound == 1) )
      {
         if ( (iEntryType == 0) || (iEntryType == 1) )
         {
            if (iMatch)
               iCheck = 1;
            else
               iStore = 1;
         }
      }
      else if ( (pEntry->d_type == DT_DIR) || (iTypeFound == 2) )
      {
         if ( (iEntryType == 0) || (iEntryType == 2) )
         {
            if ( (iMatch) && (iEntryType == 2) )
              iCheck = 1;
            else
               iStore = 1;
         }
      }

      if (iDebug == 2) fprintf(fLogFile,
         "    %d: entry %s (check %d, store %d, type %d) \n",
         iEntries, pEntry->d_name, iCheck, iStore, pEntry->d_type);

      /* check if matching with input mask */
      if (iCheck) 
      {
         /* last arg FNM_FILE_NAME: not resolved! 0 seems okay */
         if (fnmatch(pcEntryMask, pEntry->d_name, 0) == 0)
         {
            iStore = 1;                   /* match, store this entry */
            iEntriesMatch++;
         }
         else
            iStore = 0;
      }

      if (iStore)
      {
         /* store new file names */
         if ( (pEntry->d_type == DT_REG) ||
              (pEntry->d_type == DT_UNKNOWN) )
         {
            if (iPrefixDir)
            {
               strcpy(psFileList->cFile, pcDirName);
               strcat(psFileList->cFile, "/");
               strcat(psFileList->cFile, pEntry->d_name);
            }
            else
               strcpy(psFileList->cFile, pEntry->d_name);

            iFileEntries++;
            iFileEntriesNew++;

            if (iDebug)
            {
               if (iFileEntriesNew == 1)
                  fprintf(fLogFile, "new files:\n");
               fprintf(fLogFile, " %d(%d): %s\n",
                  iFileEntriesNew, iFileEntries, psFileList->cFile);
            }

            if (iFileEntryRem <= 0)
            {
  /*           if (iEntryLoop == 2)
               {
                  fprintf(fLogFile,
                     "-W- recursive file search: max size of file list buffer reached (%d entries)\n",
                     iFileEntryMax);
                  goto gEndDirEntryClose;
               }

   */          iFileEntryMax += MAX_FILE_NO;
               /* iFileEntryMax += 2;  DDDMax  */

               ii = sizeof(int) + iFileEntryMax*iFileList;
               if ((pcFileList2 = (char *) calloc((unsigned) ii, 1)) == NULL)
               {
                  fprintf(fLogFile,
                     "-E- %s: allocating new filelist buffer (%d byte, %d entries)\n",
                     cModule, ii, iFileEntryMax);
                  if (errno)
                  {
                     fprintf(fLogFile, "    %s\n", strerror(errno));
                     errno = 0;
                  }

                  iRCE = -1;
                  goto gEndDirEntryList;
               }

               if (iDebug) fprintf(fLogFile,
                  "    new filelist buffer allocated (%d byte, %d entries)\n",
                  ii, iFileEntryMax); 

               iFileEntryRem = iFileEntryMax - iFileEntries;
               piFileList2 = (int *) pcFileList2;
               *piFileList2 = iFileEntries;
               *ppFileList = (char *) piFileList2;
               piFileList2++;

               ii = iFileEntries*iFileList;
               memcpy(piFileList2, piFileList, ii);

               psFileList = (srawFileList *) piFileList2;
               psFileList0 = psFileList;

               if (iDebug) fprintf(fLogFile,
                  "    %d filelist entries copied, first %s\n",
                  iFileEntries, psFileList->cFile);
               fflush(fLogFile);

               psFileList += iFileEntries;
               piFileList = piFileList2;
               piFileList2--;
               piFileNo = piFileList2;

            } /* (iFileEntryRem <= 0) */
            else
               psFileList++;

            iFileEntryRem--;

         } /* files */

         /* store new subdirectory names */
         if (pEntry->d_type == DT_DIR)
         {
            strcpy(psDirList->cFile, pEntry->d_name);
            iDirEntries++;
            iDirEntriesNew++;
            iDirEntryRem--;

            if (iDebug)
            {
               if (iDirEntriesNew == 1)
                  fprintf(fLogFile, "new subdirs:\n");
               fprintf(fLogFile, " %d(%d): %s\n",
                  iDirEntriesNew, iDirEntries, psDirList->cFile);
            }

            if (iDirEntryRem <= 0)
            {
               fprintf(fLogFile, "DDD2 new space must be allocated\n"); 
            }

            psDirList++;

         } /* subdirs */
      } /* current entry matches and is of requested type */
   }

gEndDirEntryClose:
   iRC = closedir(pDir);

   if (iDebug)
      fprintf(fLogFile, "    rc(closedir) = %d\n", iRC);

   if ( (iDebug) || (iEntryLoop != 2) ) /* if recursive only if debug */
   {
      fprintf(fLogFile,
         "    %d entries in directory %s found\n",
         iEntries, pcDirName);
      if (iMatch) fprintf(fLogFile,
         "    %d matching with %s\n", iEntriesMatch, pcEntryMask);
   }

   if (iDebug)
   {
      if (iFileEntries) fprintf(fLogFile,
         "    thereof %d files (%d new)\n",
         iFileEntries, iFileEntriesNew);
      if (iDirEntries) fprintf(fLogFile,
         "    thereof %d subdirectories (%d new)\n",
         iDirEntries, iDirEntriesNew);
   }

   if ( (iEntryType == 0) || (iEntryType == 1) )
      *piFileNo = iFileEntries;
   if ( (iEntryType == 0) || (iEntryType == 2) )
      *piDirNo = iDirEntries;

   if (iDebug)
   {
      if (iFileEntries) fprintf(fLogFile,
         "    %d files provided in list, first %s\n",
         iFileEntries, psFileList0->cFile);
      else if ( (iEntryType == 0) || (iEntryType == 1) )
      {
         fprintf(fLogFile, "    no files in %s found",
            pcDirName);
         if (iMatch) fprintf(fLogFile,
            " matching with %s\n", pcEntryMask);
         else
            fprintf(fLogFile, "\n");
      }

      if (iDirEntries) fprintf(fLogFile,
         "    %d subdirs provided in list, first %s\n",
         iDirEntries, psDirList0->cFile);
      else if ( (iEntryType == 0) || (iEntryType == 2) )
      {
         fprintf(fLogFile, "    no subdirs in %s found",
            pcDirName);
         if ( (iMatch) && (iEntryType == 2) ) fprintf(fLogFile,
            " matching with %s\n", pcEntryMask);
         else
            fprintf(fLogFile, "\n");
      }
   }

gEndDirEntryList:

   if (iDebug)
      fprintf(fLogFile, "\n-D- end %s\n\n", cModule);

   if (iRCE)
      return iRCE;

   if (iMatch)
      return iEntriesMatch;
   return iEntries;

} /* rawGetDirEntryList */
#endif

#ifndef Lynx
/********************************************************************
 * rawGetFSEntries:
 *    get number of entries in file system (via scandir)
 *
 * created  6. 6.2002, Horst Goeringer
 ********************************************************************/

int rawGetFSEntries(char *pcStageFS)
                 /* struct dirent *(fEntryList[]) */
{
   char cModule[32]="rawGetFSEntries";
   int iDebug = 0;

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
         "-E- %s: calling system function scandir, rc=%d\n",
         cModule, iRC);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

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
#endif

/********************************************************************
 * rawGetFSfree: get free space (bytes) in specified filesystem
 *
 * created 4.2.98, Horst Goeringer
 ********************************************************************/

int rawGetFSfree(char *pcStageFS)
{
   char cModule[32]="rawGetFSfree";
   int iDebug = 0;

   int iRC;
   unsigned int itemsize, itemno;
   int iFree = 0;

   FILE *fPipe;
   int iBuf;
   char cName[BUFSIZE_SMALL] = "", *pName;
   char cBuf[BUFSIZE_SMALL] = "", *pBuf;
   char cTempFile[MAX_FULL_FILE] = "";
   char cCmd[256] = "/home/rawserv/rawdf.sh ";

   time_t tTime;
   pid_t pstr;

   pBuf = &cBuf[0];
   pName = &cName[0];
   strcpy(cTempFile, pcStageFS);

   tTime = time(NULL);
   pstr = getpid();
   sprintf(cName, "/size.t%d.p%d", (int) tTime, pstr);
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
      fprintf(fLogFile, "-E- %s: opening file %s\n",
         cModule, cTempFile);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

      return -1;
   }

   if (iDebug)
      fprintf(fLogFile, "    file %s opened\n", cTempFile);

   itemsize = sizeof(char);
   itemno = BUFSIZE_SMALL;
   iBuf = fread(pBuf, itemsize, itemno, fPipe);
   if (iBuf <= 0)
   {
      fprintf(fLogFile, "-E- %s: fread, rc = %d\n", cModule, iBuf);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

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
      fprintf(fLogFile, "-E- %s: rc = %d closing file\n",
         cModule, iRC);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

      return(-1);
   }

   if (iDebug)
      fprintf(fLogFile, "    file %s closed\n", cTempFile);

   iRC = remove(cTempFile);
   if (iRC)
   {
      fprintf(fLogFile, "-E- %s: rc = %d removing file %s\n",
         cModule, iRC, cTempFile);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

      return -1;
   }
   if (iDebug)
      fprintf(fLogFile, "    file %s removed\n", cTempFile);

   if (iDebug)
      fprintf(fLogFile, "-D- end %s\n\n", cModule);

   if (iBuf <= 0)
      return -1;
   else
      return(iFree);

} /* rawGetFSfree */

#ifndef Lynx
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
         "-E- %s: calling system function statfs, rc=%d\n",
         cModule, iRC);
      if (errno)
      {
         fprintf(fLogFile, "    %s\n", strerror(errno));
         errno = 0;
      }

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

int rawGetFileAttr(char *pcFile, unsigned long *plFileSize)
{
   char cModule[32] = "rawGetFileAttr";
   int iDebug = 0;

   FILE *f_ifile;
   int iRC;
   unsigned long lFileSize;
   unsigned long lr;
   int iReclen;

   int ilocSize = 5;               /* token no. 5 contains file size */
   int ilocFile = 9;       /* token no. 9 contains file name (check) */
   int ii, ilen, iloc;

   char cCmd[CMDLEN] = "ls -l ";
   char *pCmd;
   const char *pType = "r";
   char *pBuf, *pBuf0;
   const char *pcDollar = "$"; /* requ. spec. treatment in file name */
   char *pcc, *ploc;

   char *pctoken;
   const char *pcblank = " ";
   /* char ctoken[20]; */
   int icount = 0;

/******************** end of declarations *****************/

   if (iDebug)
   {
      fprintf(fLogFile,
         "\n-D- begin %s: file %s\n", cModule, pcFile);
      fflush(fLogFile);
   }

   iReclen = 0;            /* not available in Unix system */ 

   /* treat $ in file name */
   ploc = strchr(pcFile, *pcDollar);
   if (ploc != NULL)
   {
      if (iDebug)
      {
         fprintf(fLogFile, "-D- $ in file name found: %s\n", pcFile);
         fflush(fLogFile);
      }
      ilen = strlen(pcFile);
      pcc = pcFile;
      for (ii = 1; ii <= ilen; ii++)
      {
         iloc = strncmp(pcc, pcDollar, 1);
         if (iloc == 0) strncat(cCmd, "\\", 1);        /* $ found */
         strncat(cCmd, pcc++, 1);
      }
   } /* ploc != NULL, $ in file name */
   else strcat(cCmd, pcFile);

   if (iDebug)
   {
      fprintf(fLogFile, "-D- %s: ex '%s'\n", cModule, cCmd);
      fflush(fLogFile);
   }

   pCmd = &cCmd[0];
   f_ifile = popen(pCmd, pType);
   if (f_ifile == NULL)
   {
      fprintf(fLogFile, "-E- %s: opening pipe\n", cModule);
      return -1;
   }
   if (iDebug)
   {
      fprintf(fLogFile, "    %s: pipe opened\n", cModule);
      fflush(fLogFile);
   }

   if ( !(pBuf0 = (char *) malloc(BUFSIZE)) )
   {
      fprintf(fLogFile, "-E- %s: allocation buffer failed\n", cModule);
      pclose(f_ifile);
      return(-1);
   }
   if (iDebug)
   {
      fprintf(fLogFile, "    %s: buffer allocated\n", cModule);
      fflush(fLogFile);
   }

   pBuf = pBuf0;
   lr = fread(pBuf, sizeof(char), BUFSIZE, f_ifile);
   if (lr <= 0)
   {
      fprintf(fLogFile, "-E- %s: reading from pipe failed\n", cModule);
      pclose(f_ifile);
      return(-1);
   }

   pBuf += (lr-1);
   strncpy(pBuf, "\0", 1);    /* overwrite newline character */

   if (iDebug)
   {
      fprintf(fLogFile, "    %s: complete string: (%lu bytes): \n%s\n",
         cModule, lr, pBuf0);
      fflush(fLogFile);
   }

   pBuf = pBuf0;
   while ( (pctoken = strtok(pBuf, pcblank)) != NULL)
   {
      icount++;
      if (iDebug)
      {
         fprintf(fLogFile, "    token %d: %s\n", icount, pctoken);
         fflush(fLogFile);
      }

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
      /* test large file size  DDD
         strcpy(ctoken, "2000000000");
         pctoken = &ctoken;
       */
         if ( ( iRC = sscanf( pctoken, "%lu", &lFileSize) ) <= 0 )
         {
            fprintf(fLogFile, "-E- %s: filesize %lu (%s) invalid\n",
               cModule, lFileSize, pctoken);
            if (errno)
            {
               fprintf(fLogFile, "    %s\n", strerror(errno));
               errno = 0;
            }
            pclose(f_ifile);

            return(-1);
         }
         if (iDebug)
         {
            fprintf(fLogFile, "    file size %lu\n", lFileSize);
            fflush(fLogFile);
         }

         if ( (lFileSize == 0) && (iDebug) )
            fprintf(fLogFile, "    file %s empty\n", pcFile);

         if (lFileSize > MAX_FILE_SIZE)
         {
            fprintf(fLogFile,
               "-E- %s: file %s too large (%lu byte), max allowed 2GB -1\n",
               cModule, pcFile, lFileSize);
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

   /* pclose provides "No child processes" only if iRC=0 */
   if (errno)
      errno = 0;

   *plFileSize = lFileSize;
   if (iDebug) 
   {
      fprintf(fLogFile, "-D- end %s\n\n", cModule);
      fflush(fLogFile);
   }

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
                    char **ppFileList)
{
   char cModule[32] = "rawGetFileList";
   int iDebug = 0;

   int iRC = 0;
   int iRCE = 0;
   int iIgnore = 0;                  /* =1: new file already in list */
   int ilen, iloc, ii, jj;
   int ird, ipc, ipc1, irem, grem = 0;
   int iFileno, iFilenoo;
   int *piFileno;              /* points to no. of files in filelist */
   int *piFileList;              /* points to first file in filelist */

   char *pcc, *pcc0, *ploc;
   unsigned long lr;

   FILE *f_ifile;

   char cCmd[CMDLEN] = "ls -L ";
                         /* mark directory names with / to skip them */
   /* char cCmd[CMDLEN] = "ls ";  was valid from 28.7.2000-18.6.2001 */
   char *pCmd;
   char *pBuf; 
   char cBuf[MAX_FULL_FILE];   /* temp buffer for incompl file names */
   const char *pcDollar = "$"; /* requ. spec. treatment in file name */

   srawFileList *psFileList,    /* points to actual file in filelist */
                *psFileList0,    /* points to first file in filelist */
                *psFileList0c;       /* current file in old filelist */

   /******************************************************************/

   if (iDebug) 
      fprintf(fLogFile, "\n-D- begin %s\n", cModule);

   ploc = strchr(pcFile, *pcDollar);
   if (ploc != NULL)
   { 
      if (iDebug)
         fprintf(fLogFile, "    '$' in file name found: %s\n", pcFile);
      ilen = strlen(pcFile);
      pcc = pcFile;
      for (ii=1; ii<=ilen; ii++)
      {
         iloc = strncmp(pcc, pcDollar, 1);
         if (iloc == 0) strncat(cCmd, "\\", 1);           /* $ found */
         strncat(cCmd, pcc++, 1);
      }
   } /* ploc != NULL, $ in file name */
   else strcat(cCmd, pcFile);
   strcat(cCmd, " 2>/dev/null");

   if (iDebug)
      fprintf(fLogFile, "    command: %s\n", cCmd);

   piFileList = (int *) *ppFileList;  /* pointer to file list buffer */
   piFileno = piFileList++; /* points to no. of files, updated later */
   iFilenoo = *piFileno;
   psFileList = (srawFileList *) piFileList;
   psFileList0 = psFileList;        /* points now to first file name */

   if (iFilenoo)                   /* skip files from previous calls */
   {
      if (iDebug) fprintf(fLogFile,
         "    %d old files in list\n", iFilenoo);

      for (ii=1; ii<=iFilenoo; ii++)
      {
         if (iDebug == 1) fprintf(fLogFile,
            "    %d (old): %s\n", ii, psFileList->cFile);
         psFileList++;
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
   if (iDebug)
      fprintf(fLogFile, "    buffer allocated\n");

   memset(&cBuf[0], '\0', MAX_FULL_FILE);
   lr = 1;
   iFileno = 0;
   while (lr > 0)
   {
gRead:
      memset(pBuf, '\0', BUFSIZE);
      lr = fread(pBuf, sizeof(char), BUFSIZE, f_ifile);
      if (lr > 0)
      {
         pcc0 = pBuf;
         pcc = pBuf;
         ird = lr;        /* meaningful bytes     */

         if (iDebug == 2)
            fprintf(fLogFile, "-D- received %d bytes:\n", ird);

         while(ird > 0)
         {
            if (iDebug == 2)
               fprintf(fLogFile, "    '%s'", pcc0);
            ipc = strcspn(pcc0, "\n");
            irem = strlen(pcc0);
            pcc = strchr(pcc0, '\n');
            if (iDebug == 2) fprintf(fLogFile,
               "\n    first length %d, total %d, remainder %d\n", 
               ipc, irem, grem);

            if (grem)     /* incompl. file name from previous buffer */
            {
               if (ipc > 0)
               {
                  strncat(cBuf, pcc0, (unsigned) ipc);
                  if (iDebug == 2) fprintf(fLogFile,
                     "    last  concatenated: %s\n", cBuf);
               }

               ii = strlen(cBuf);

               /* after file names: subdir (marked by trailing ":") */
               if (strncmp(&(cBuf[ii-1]), ":", 1) == 0)
               {
                  if (iDebug) fprintf(fLogFile,
                     "\n    stop checking, ignore subdirectory %s\n",
                     cBuf);
                  lr = 0;
                  break;
               }

               if ( (strcmp(cBuf, "./")) &&           /* ignore "./" */
                    (ii) &&                   /* ignore empty string */
                    (strncmp(&(cBuf[ii-1]), ":", 1)) )
                                     /* ignore subdir (trailing ":") */
               {
                  iRC = rawTestFileName(cBuf);
                  if ( (iRC == 0) ||
                       ( (iRC == 1) && (iAccept >= 1) ) )
                  {
                     if (iFilenoo)
                     {
                        psFileList0c = psFileList0; /* first old file */

                        /* compare new name with old ones */
                        for (jj=1; jj<=iFilenoo; jj++)
                        {
                           iRC = strcmp(cBuf, psFileList0c->cFile);
                           if (iRC == 0)
                           {
                              iIgnore = 1;
                              if (iDebug) fprintf(fLogFile,
                                 "    entry %s already available(1)\n",
                                 cBuf);
                              break;
                           }
                           psFileList0c++;
                        }
                     } /* (iFilenoo) */ 

                     if (iIgnore == 0)
                     {
                        strcpy(psFileList->cFile, cBuf);
                        iFileno++;
                        if (iDebug) fprintf(fLogFile,
                           "    %s stored(1), addr %p\n",
                           psFileList->cFile, psFileList);

                        psFileList++;
                     }
                     else
                        iIgnore = 0;

                     if (iFileno >= MAX_FILE_NO)
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
               strncpy(cBuf, pcc0, (unsigned) ipc);
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

                  /* after file names: subdir (marked by trailing ":") */
                  if (strncmp(&(cBuf[ii-1]), ":", 1) == 0)
                  {
                     if (iDebug) fprintf(fLogFile,
                        "\n    stop checking, ignore subdirectory %s\n",
                        cBuf);
                     lr = 0;
                     break;
                  }

                  if ( (strcmp(cBuf, "./")) &&  /* ignore "./" */
                       (ii) &&          /* ignore empty string */
                       (strncmp(&(cBuf[ii-1]), ":", 1)) )
                               /* ignore subdir (trailing ":") */
                  {
                     iRC = rawTestFileName(cBuf);
                     if ( (iRC == 0) ||
                          ( (iRC == 1) && (iAccept >= 1) ) )
                     {
                        if (iFilenoo)
                        {
                           psFileList0c = psFileList0;/* 1st old file */

                           /* compare new name with old ones */
                           for (jj=1; jj<=iFilenoo; jj++)
                           {
                              if (ipc == (int) strlen(psFileList0c->cFile))
                              {
                                 iRC = strncmp(cBuf,
                                               psFileList0c->cFile,
                                               (unsigned) ipc);
                                 if (iRC == 0)
                                 {
                                    iIgnore = 1;
                                    if (iDebug) fprintf(fLogFile,
                                       "    entry %s already available(2)\n",
                                       cBuf);
                                    break;
                                 }
                              }
                              psFileList0c++;
                           }
                        } /* (iFilenoo) */

                        if (iIgnore == 0)
                        {
                           strncpy(psFileList->cFile, cBuf, (unsigned) ipc);
                           iFileno++;

                           if (iDebug) fprintf(fLogFile,
                              "    %s stored(2), addr %p\n",
                              psFileList->cFile, psFileList);
                           psFileList++;
                        }
                        else
                           iIgnore = 0;

                        if (iFileno >= MAX_FILE_NO)
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

            pcc0 = ++pcc;
            ird -= (ipc+1);
         } /* while(ird > 0) */

      } /* if(lr > 0) */

   } /* while(lr > 0) */

gFinishList:
   if (iEntryLoop)
   {
      if (iFileno == 0)
         fprintf(fLogFile, "-I- no (new) files found\n");
      else if (iFileno > 1)
         fprintf(fLogFile, "-I- %d (new) files found\n", iFileno);
   }

   if ( (iDebug) && (iFileno) )
   {
      psFileList = (srawFileList *) piFileList;
                                    /* points now to first file name */
      for (ii=1; ii<=iFilenoo; ii++)
      {
         if (iDebug == 1) fprintf(fLogFile,
            "    %d (old): %s\n", ii, psFileList->cFile); psFileList++;
      }
      for (ii=iFilenoo+1; ii<=iFilenoo+iFileno; ii++)
      {
         if (iDebug == 1) fprintf(fLogFile,
            "    %d: %s\n", ii, psFileList->cFile); psFileList++;
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
         if (errno)
         {
            fprintf(fLogFile, "    %s\n", strerror(errno));
            errno = 0;
         }
      }
      else
         fprintf(fLogFile, "    pipe closed\n");

      fprintf(fLogFile, "-D- end %s\n\n", cModule);
   }

   /* pclose provides "No child processes" only if iRC=0 */
   if (errno)
      errno = 0;

   if (iRCE)
      return iRCE;
   else
      return 0;

} /* rawGetFileList */

/**********************************************************************
 * rawGetHostConn:   get network connection type of client host
 * created 19.3.98, Horst Goeringer
 **********************************************************************/

int rawGetHostConn()
{
   char cModule[32] = "rawGetHostConn";
   int iDebug = 0;

   int iRC;
   int ii;
   int iBuf = 0;
   int iCol = 0;                    /* no. of columns in cmd output */
   int iType = 0;                /* network connection type
                                    = 1: ethernet (slow)
                                    = 2: ethernet (fast, nodes linux*
                                    = 3: fddi
                                    = 4: SP switch */
   char *pcc;
   char cToken[16] = "", *pToken;
   char cheadName[16], *pheadName;
   char cheadMtu[16] = "mtu",  *pheadMtu;
   char cName[16] = "", *pName;
   char cMtu[16] = "",  *pMtu;
   char cNameRef1[16] = "";
   char cNameRef2[16] = "";

   char cCmd[CMDLEN] = "netstat -i", *pCmd;
   char cBuf[1024] = "", *pBuf;
   char cNode[MAX_NODE] = "";

   FILE *f_ifile;

   if (iDebug)
      fprintf(fLogFile, "\n-D- in %s\n", cModule);

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
   if (iDebug) fprintf(fLogFile,
      "    %s: client host %s\n", cModule, cNode);

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
   if (iDebug)
      fprintf(fLogFile, "    %s: pipe opened\n", cModule);

   iBuf = fread(pBuf, sizeof(char), 1024, f_ifile);
   if (iBuf <= 0)
   {
      fprintf(fLogFile, "-E- %s: reading from pipe failed\n", cModule);
      goto gError;
   }

   if (iDebug == 2)
      fprintf(fLogFile, "    %s command output: \n%s", cModule, cBuf);

   pToken = strtok(pBuf, " \n");
   pcc = pToken;
   while (*pcc != '\0')
   {
      *pcc = tolower(*pcc);
      pcc++;    /* gcc 3.3.5: need two steps, else string corrupted */
   }

   if (strcmp(pToken, pheadName))
   {
      fprintf(fLogFile, "-E- %s: invalid name heading (%s, expected %s)\n",
              cModule, pToken, pheadName);
      goto gError;
   }

   pToken = strtok(NULL, " \n");
   pcc = pToken;
   while (*pcc != '\0')
   {
      *pcc = tolower(*pcc);
      pcc++;    /* gcc 3.3.5: need two steps, else string corrupted */
   }

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
      pcc = pToken;
      while (*pcc != '\0')
      {
	 *pcc = tolower(*pcc);
         pcc++; /* gcc 3.3.5: need two steps, else string corrupted */
      }

      if (iDebug == 2)
         fprintf(fLogFile, "DDD %s: %s\n", cModule, pToken);

      if (strcmp(pToken, cNameRef1) == 0)
      {
         iType = 1;
         if (iDebug)
            fprintf(fLogFile, "    %s: ethernet available\n", cModule);
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
      fprintf(fLogFile, "-D- end %s: network connection type %d\n\n",
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
   char cModule[32] = "rawGetUserid";
   int iDebug = 0;

   unsigned int lr;
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
   if (iDebug)
      fprintf(fLogFile, "    %s: pipe opened\n", cModule);

   if ( !(pBuf0 = (char *) malloc(BUFSIZE)) )
   {
      fprintf(fLogFile, "-E- %s: allocation buffer failed\n", cModule);
      pclose(f_ifile);
      return(NULL);
   }
   if (iDebug)
      fprintf(fLogFile, "    %s: buffer allocated\n", cModule);

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

   if (iDebug) fprintf(fLogFile,
      "-D- %s: user name (%lu bytes): %s\n", cModule, lr, pBuf0);

   return(pBuf0);

} /* rawGetUserid */
