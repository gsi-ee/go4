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
 * 29. 3.2006, H.G.: cNodeMasterC: gsitsma -> lxgstore
 * 28. 4.2006, H.G.: remove node/port of entry server (-> rawcommn.h)
 * 11.11.2008, H.G.: declarations 'char *' -> 'const char *'
 *  3.12.2008, H.G.: pcDevDelim: 'char *' -> 'const char *'
 *********************************************************************
 */

#define CMDLEN 256      /* max length of commands (shell or clients) */
#define BUFSIZE 256     /* buffer size reading temporary (pipe) data */

static const char *pcDevDelim = ":";
#ifdef VMS
   static const char *pcFileDelim = "]";
   static const char *pcFileDelim0 = "[";
   static const char *pcFileDelim2 = ":";
   static const char *pcObjDelim = "/";
   static const char *pcObjDelimAlt = "\\";
#endif

#ifdef _AIX
   static const char *pcFileDelim = "/";
   static const char *pcObjDelim = "/";
   static const char *pcObjDelimAlt = "\\";
#endif

#ifdef Linux
   static const char *pcFileDelim = "/";
   static const char *pcObjDelim = "/";
   static const char *pcObjDelimAlt = "\\";
#endif

#ifdef Lynx
   static const char *pcFileDelim = "/";
   static const char *pcObjDelim = "/";
   static const char *pcObjDelimAlt = "\\";
#endif

#ifdef WIN32
   static const char *pcFileDelim = "\\";
   static const char *pcObjDelim = "\\";
   static const char *pcObjDelimAlt = "/";
#endif

static const char *pcStar = "*";             /* wildcard char in TSM */
static const char *pcQM = "?";               /* wildcard char in TSM */
static const char *pcPerc = "%";             /* wildcard char in VMS */
static const char *pcOpt1 = "-";          /* command option marker 1 */
static const char *pcOpt2 = "/";    /* command option marker 2 (VMS) */
static int iOptm = 1;                     /* length of option marker */
static int iOpt = 1;            /* length of option (without marker) */

