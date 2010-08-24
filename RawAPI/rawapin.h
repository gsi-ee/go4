/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created  1.12.2000 by Horst Goeringer
 *********************************************************************
 * rawapin.h
 *   entry definitions for client API
 * new version
 *********************************************************************
 *  7. 2.2001, H.G.: rfio_gsi_query added
 * 26. 2.2001, H.G.: rfio_fstat, rfio_perror, rfio_serror added
 *                   for ROOT version 300-05
 * 15.10.2002, H.G.: remove declaration of fLogFile
 * 13. 1.2003, H.G.: remove definition PORT_MOVER (in rawcommn.h)
 * 12. 6.2003, H.G.: add externals fLogFile, piFilelist
 *  4. 8.2004, H.G.: new entries rfio_[f]endfile, rfio_[f]newfile
 * 17. 2.2005, H.G.: add C++ binding
 * 20. 4.2005, H.G.: mod C++ binding for rootcint
 * 22. 5.2007, H.G.: new entries rfio_open64, rfio_lseek64
 * 29. 5.2007, H.G.: new entries rfio_stat64, rfio_fstat64
 * 14.11.2008, H.G.: new entry rfio_cache_stat
 * 17.11.2008, H.G.: new entr. rfio_preseek64, rfio_mkdir, rfio_closedir
 *                   mod file args from 'char *' to 'const char *' in:
 *                   rfio_access, rfio_fwrite, rfio_open, rfio_open64,
 *                   rfio_unlink, rfio_write
 * 11. 2.2009, H.G.: new entries rfio_open_gsidaq, rfio_fopen_gsidaq
 * 26. 6.2009, H.G.: for 64 bit client: replace int args: 
 *                   rfio_fread: return, args 2, 3 by size_t
 *                   rfio_fwrite: return, args 2, 3 by size_t
 *                   rfio_read: arg 3 by size_t, return by ssize_t
 *                   rfio_write: arg 3 by size_t, return by ssize_t
 * 26. 2.2010, H.G.: rfio_gsi_query: add parameter (len output string)
 *                   include unistd.h, if not yet done
 * 23. 4.2010, H.G.: new entry rfio_fopen_gsidaq_dm
 *********************************************************************
 */

#ifndef RAWAPI__H
#define RAWAPI__H

#ifdef Lynx

#ifndef USER_API
#define USER_API
FILE *fLogFile;                      /* external in some C functions */
#endif

#else  /* not Lynx */

#ifndef __cplusplus
FILE *fLogFile;                      /* external in some C functions */
#endif

#ifndef _STDINT_H
#include <stdint.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#endif  /* not Lynx */

#include "rawcommn.h"
#include "rawclin.h"

#define RFILE srawAPIFile

/****************** entries using 64 bit functions *******************/

#ifdef __USE_LARGEFILE64

#ifdef __cplusplus
extern "C" {
#endif
int rfio_open64(const char *, int, int);
   /* open connection to server and file */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int64_t rfio_lseek64(int, int64_t, int);
   /* move read/write pointer */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_stat64(const char *, struct stat64 *);
   /* get large file status */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_fstat64(int, struct stat64 *);
   /* get large file status */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_preseek64(int, const struct iovec64 *, int);
   /* move read/write pointer */
#ifdef __cplusplus
}
#endif

#endif /* __USE_LARGEFILE64 */

/********************* entries using file ptr ************************/

#ifdef __cplusplus
extern "C" {
#endif
RFILE *rfio_fopen_gsidaq_dm(
         char *, char *, char *, int *, int, char *, int, int, int);
   /* open connection to gStore server and file and prepare copy
      to lustre or read cache before migration to tape,
      provide data mover name and running no. */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
RFILE *rfio_fopen_gsidaq(char *, char *, int, char *, int, int, int);
   /* open connection to gStore server and file and prepare copy
      to lustre or read cache before migration to tape */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
RFILE *rfio_fopen(char *, char *);
   /* open connection to gStore server and file */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
size_t rfio_fread(char *, size_t, size_t, RFILE *);
   /* read file buffer */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
size_t rfio_fwrite(const char *, size_t, size_t, RFILE *);
   /* write file buffer */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_fclose(RFILE *);
   /* close connection to server and file */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_fendfile(RFILE *);
   /* close file, keep connection to server */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_fnewfile(RFILE *, char *);
   /* open new file on already connected server */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_mkdir(const char *, int);
   /* open new file on already connected server */
#ifdef __cplusplus
}
#endif

/********************** entries using file id ********************************/

#ifdef __cplusplus
extern "C" {
#endif
int rfio_open_gsidaq(const char *, int, int, int, char *, int, int, int);
   /* open connection to gStore server and file and prepare copy
      to lustre or read cache before migration to tape */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_open(const char *, int, int);
   /* open connection to gStore server and file */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
ssize_t rfio_read(int, char *, size_t);
   /* read file buffer */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
ssize_t rfio_write(int, const char *, size_t);
   /* write file buffer */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_close(int);
   /* close connection to server and file */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_endfile(int);
   /* close file, keep connection to server */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_newfile(int, char *);
   /* open new file on already connected server */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_lseek(int, int, int);
   /* move read/write pointer */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_fstat(int, struct stat *);
   /* get file status (still dummy) */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
void rfio_perror(const char *);
   /* print error message */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
char *rfio_serror();
   /* get error message */
#ifdef __cplusplus
}
#endif

/**************************** other entries **********************************/

#ifdef __cplusplus
extern "C" {
#endif
int rfio_cache_stat(const char *);
   /* get query info for remote file already opened */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_gsi_query(int, int, int, char *);
   /* get query info for remote file already opened */
#ifdef __cplusplus
}
#endif

/********************** (still) dummy entries ********************************/

#ifdef __cplusplus
extern "C" {
#endif
int rfio_access(const char *, int);
   /* get access status (still dummy) */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_stat(const char *, struct stat *);
   /* get file status (still dummy) */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_parse(char *, char **, char **);
   /* parse name to host and path (still dummy) */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_unlink(const char *);
   /* (still dummy) */
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
int rfio_closedir(void *);
   /* (still dummy) */
#ifdef __cplusplus
}
#endif

#endif /* RAWAPI__H */
