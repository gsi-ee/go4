// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/* This central include file defines data types for all platforms */

#ifndef TYPEDEF_H
#define TYPEDEF_H

/* Platform independent definitions */

typedef          char   CHARS;
typedef unsigned char   CHARU;
typedef char            CHARX;
typedef          char   INTS1;
typedef unsigned char   INTU1;
typedef          short  INTS2;
typedef unsigned short  INTU2;
typedef          int    INTS4;
typedef unsigned int    INTU4;
typedef          float  REAL4;
typedef          double REAL8;

/* Platform specific definitions */

#ifdef WIN32
#define MBS_ENDIAN 0 /* little endian               */
#define GSI__WINNT
#include "typedefs_nt.h"
typedef unsigned long ADDRS;
#endif


#ifdef Linux
#define MBS_ENDIAN 0 /* little endian               */
#define GSI__LINUX
#define GSI__UNIX
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#endif

#ifdef Solaris
#define MBS_ENDIAN 1 /* big endian         */
#define GSI__SOLARIS
#define GSI__SLARIS
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#endif


#ifdef Darwin
#define MBS_ENDIAN 0 /* little endian               */
#define GSI__LINUX
#define GSI__UNIX
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#endif


#ifdef _AIX
#define MBS_ENDIAN 1 /* big    endian */
#define GSI__AIX
#define GSI__UNIX
typedef          long long INTS8;
typedef unsigned long long INTU8;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
typedef unsigned long ADDRS;
#endif

#ifdef __osf__
#ifdef __alpha
#define MBS_ENDIAN 1 /* big    endian */
#define AXP
#define GSI__UNIX_AXP
#define GSI__UNIX
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#endif
#endif

#ifdef _HPUX_SOURCE
#define MBS_ENDIAN  1 /* big    endian               */
#define GSI__HPUX
#define GSI__UNIX
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#endif

#ifdef Lynx
#define MBS_ENDIAN  1 /* big    endian               */
#define GSI__LYNX
#define GSI__UNIX
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#endif

#ifdef VMS
#define MBS_ENDIAN  0  /* little endian               */
#define GSI__VMS
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
typedef unsigned long long ADDR64; // JAM for 64 bit status structure receiving
#ifdef __alpha
#define GSI__VMS_AXP
#define AXP
#endif
#ifdef VAX
#define GSI__VMS_VAX
#endif
#endif


#endif
