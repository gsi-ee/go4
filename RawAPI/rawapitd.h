/*********************************************************************
 * rawapitd.h
 * ADSM definitions from dsmapitd.h, needed also for clients
 * if a new ADSM API version is introduced, the following definitions
 * must be updated according to those in the new dsmapitd.h
 * okay for v216 and v315
 *
 * created 19.10.98, Horst Goeringer
 *********************************************************************
 * 22. 7.2002, H.G.: okay for version 5.1
 *********************************************************************
 */

#define DSM_MAX_DESCR_LENGTH          255       /* archive description    */
#define DSM_MAX_OWNER_LENGTH          64        /* object owner name      */
#define DSM_MAX_FSNAME_LENGTH         1024      /* filespace name         */
#define DSM_MAX_HL_LENGTH             1024      /* object high level name */
#define DSM_MAX_LL_LENGTH             256       /* object low level name  */

#define  MEDIA_FIXED          0x10
#define  MEDIA_LIBRARY        0x20
#define  MEDIA_NETWORK        0x30
#define  MEDIA_SHELF          0x40
#define  MEDIA_OFFSITE        0x50
#define  MEDIA_UNAVAILABLE    0xF0


typedef enum
{
        bFalse = 0x00,
        bTrue  = 0x01
} bool_t;

typedef          char  dsInt8_t;          /* v315 and higher */
typedef unsigned char  dsUint8_t;         /* v315 and higher */

typedef signed   char     int8   ;        /* v216 and below */
typedef unsigned char     uint8  ;        /* v216 and below */


/*-------------------------------------------------------------------------+
|  Type definition for object name used on BindMC, Send, Delete, Query     |
+-------------------------------------------------------------------------*/
typedef struct S_dsmObjName
{
    char   fs[DSM_MAX_FSNAME_LENGTH + 1] ;              /* Filespace name */
    char   hl[DSM_MAX_HL_LENGTH + 1] ;                 /* High level name */
    char   ll[DSM_MAX_LL_LENGTH + 1] ;                  /* Low level name */
    uint8  objType;          /* for object type values, see defines above */
} dsmObjName;


