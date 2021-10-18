// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/* filter structure for CLIENT */

#ifndef S_CLNT_FILTER_H
#define S_CLNT_FILTER_H

#include "gps_sc_def.h"

#if MBS_ENDIAN == 1

struct s_clnt_filter
      {
/* -------> Swapped <-------- */
/* --------=========--------- */
      /* --- control byte ordering and machine type (2LW) ---- */
      unsigned long          l_testbit;       /* bit pattern               */
      unsigned long          l_endian;        /* endian of sender          */
      /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      long                   l_numb_of_evt;   /* numb of events to send    */
      long                   l_sample_rate;   /* flt match sample rate     */
      long                   l_flush_rate;    /* buffer flushing rate [sec]*/
      struct s_filter        filter[GPS__MAXFLT];/* 32 filter express (3LW) */
      struct s_flt_descr     flt_descr[GPS__MAXFLTDESCR]; /* Filter descriptor*/
      short unsigned         if_fltsev;      /* filter on subevent         */
      short unsigned         if_fltevt;      /* filter on event            */
      short unsigned         if_wrtsev;      /* write subevts (numb of sev)*/
      short unsigned         if_wrtevt;      /* write whole event          */
      };

#else

struct s_clnt_filter
      {
      /* --- control byte ordering and machine type (2LW) ---- */
      unsigned int          l_testbit;       /* bit pattern               */
      unsigned int          l_endian;        /* endian of sender          */
      /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      int                   l_numb_of_evt;   /* numb of events to send    */
      int                   l_sample_rate;   /* flt match sample rate     */
      int                   l_flush_rate;    /* buffer flushing rate [sec]*/
      struct s_filter        filter[GPS__MAXFLT];/* 32 filter express (3LW) */
      struct s_flt_descr     flt_descr[GPS__MAXFLTDESCR]; /* Filter descriptor*/
      short unsigned         if_fltevt;      /* filter on event            */
      short unsigned         if_fltsev;      /* filter on subevent         */
      short unsigned         if_wrtevt;      /* write whole event          */
      short unsigned         if_wrtsev;      /* write subevts (numb of sev)*/
      };

#endif

#endif
