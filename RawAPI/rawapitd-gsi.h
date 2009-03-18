/********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 12. 9.1997 by Horst Goeringer
 ********************************************************************
 * dsmapitd-gsi.h
 *    GSI extensions to adsm type definitions due to central staging
 *********************************************************************
 * 29. 5.2002, H.G.: add GSI_MEDIA_INCOMPLETE
 * 19. 9.2003, H.G.: add GSI_MEDIA_CACHE
 *********************************************************************
 */

/* additional values for respArchive.mediaClass:
                                             (standard ADSM: < 0x100) */
#define  GSI_MEDIA_STAGE      0x1001/* file available in staging pool */
#define  GSI_MEDIA_LOCKED     0x1002          /* file locked for read */
#define  GSI_MEDIA_INCOMPLETE 0x1003  /* file still locked for create */
#define  GSI_MEDIA_CACHE      0x1004 /* file available in write cache */

