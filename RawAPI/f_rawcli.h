/*********************************************************************/
/* f_rawcli.h  */
/* function definition of client to API ADSM client */
/* created 30. 5.96, Horst Goeringer */
/*********************************************************************/
/* 20. 9.96, H.G.: new keyword append (local tape) */
/*********************************************************************/
/* */
INTS4 f_rawcli(CHARS *pcAction,
               CHARS *pcFile,
               CHARS *pcArchive,
               CHARS *pcPath,
               INTS4 lReclen,
               CHARS *pcOwner,
               CHARS *pcNode,
               INTS4 lPortNo,
               CHARS *pcDevice,
               INTS4 lAppend,
               INTS4 lDebugLevel);

