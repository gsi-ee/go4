/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 12. 9.1997 by Horst Goeringer
 ********************************************************************
 * rawapitd-gsin.h
 *    GSI extensions to TSM type definitions for central disk cache
 *********************************************************************
 * 29. 5.2002, H.G.: add GSI_MEDIA_INCOMPLETE
 * 19. 9.2003, H.G.: add GSI_MEDIA_CACHE
 * 25. 1.2005, H.G.: separate values ..._LOCKED and ..._INCOMPLETE for
 *                   stage and (write) cache files
 * 12.12.2007, H.G.: add GSI_STAGE_INVALID, GSI_CACHE_INVALID
 * 11. 5.2009, H.G.: add GSI_CACHE_COPY
 *********************************************************************
 */

/* additional values for respArchive.mediaClass:
                                             (standard ADSM: < 0x100) */
#define  GSI_MEDIA_STAGE      0x1001/* file available in staging pool */
#define  GSI_MEDIA_LOCKED     0x1002    /* stage file locked for read */
#define  GSI_MEDIA_INCOMPLETE 0x1003  /* stage file locked for create */
#define  GSI_STAGE_INVALID    0x1004     /* stage file lock timed out */
#define  GSI_MEDIA_CACHE      0x1011 /* file available in write cache */
#define  GSI_CACHE_LOCKED     0x1012    /* cache file locked for read */
#define  GSI_CACHE_INCOMPLETE 0x1013  /* cache file locked for create */
#define  GSI_CACHE_INVALID    0x1014     /* cache file lock timed out */
#define  GSI_CACHE_COPY       0x1015
                   /* write cache file lock for copy before migration */
