/*********************************************************************
 * Copyright:
 *   GSI, Gesellschaft fuer Schwerionenforschung mbH
 *   Planckstr. 1
 *   D-64291 Darmstadt
 *   Germany
 * created 11. 6.1996 by Horst Goeringer
 *********************************************************************
 * rawdefn.h
 *   some common definitions for LOCAL declarations
 *   new version
 *********************************************************************
 * 29.10.1996, H.G.: server node sp2b01_f
 * 21. 4.1997, H.G.: wildcard character %
 * 20. 5.1997, H.G.: CMDLEN and BUFSIZE added
 * 10. 7.1997, H.G.: alias name adsmexp for server node
 *  7.11.2001, H.G.: ported to W2000
 * 14.10.2002, H.G.: ported to Lynx
 * 31. 1.2003, H.G.: created from rawdef.h, default master: depc134
 *  6.10.2003, H.G.: cNodeServ ("depc134") => cNodeMasterC ("gsitsma")
 *********************************************************************
 */

#define CMDLEN 256      /* max length of commands (shell or clients) */
#define BUFSIZE 256     /* buffer size reading temporary (pipe) data */

static char cNodeMasterC[16] = "gsitsma";        /* for m_tsm.c only */
static int iPortServ = 1996;

static char *pcDevDelim = ":";
#ifdef VMS
   static char *pcFileDelim = "]";
   static char *pcFileDelim0 = "[";
   static char *pcFileDelim2 = ":";
   static char *pcObjDelim = "/";
   static char *pcObjDelimAlt = "\\";
#endif

#ifdef _AIX
   static char *pcFileDelim = "/";
   static char *pcObjDelim = "/";
   static char *pcObjDelimAlt = "\\";
#endif

#ifdef Linux
   static char *pcFileDelim = "/";
   static char *pcObjDelim = "/";
   static char *pcObjDelimAlt = "\\";
#endif

#ifdef Lynx
   static char *pcFileDelim = "/";
   static char *pcObjDelim = "/";
   static char *pcObjDelimAlt = "\\";
#endif

#ifdef WIN32
   static char *pcFileDelim = "\\";
   static char *pcObjDelim = "\\";
   static char *pcObjDelimAlt = "/";
#endif

static char *pcStar = "*", *pcQM = "?";     /* wildcard characters in ADSM */
static char *pcPerc = "%";                  /* used for ? in user interface */
static char *pcOpt1 = "-";                      /* command option marker 1 */
static char *pcOpt2 = "/";            /* command option marker 2 (VMS only) */
static int iOptm = 1;                            /* length of option marker */
static int iOpt = 1;                   /* length of option (without marker) */




