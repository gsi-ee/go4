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

#ifndef S_FLT_DESCR_H
#define S_FLT_DESCR_H

#include "typedefs.h"

#if MBS_ENDIAN == 1

/* filter descriptor */
/* -------> Swapped <-------- */
/* --------=========--------- */
struct s_flt_descr {
      /* indices to filter[]  */
      char                   h_fltblkend;       /* end filter block          */
      char                   h_fltblkbeg;   /* begin filter block        */
      char                   hf_fltdescr;       /* filter descriptor         */
      char                   hf_wrtdescr;   /* write descriptor          */
      /* index to flt_descr[] */
      short                  i_descriptors;     /* number of descriptors     */
      char                   h_dummy;
      char                   h_nextdescr;       /* next descriptor           */
      };


#else

/* filter descriptor */
struct s_flt_descr
      {
      char                   hf_wrtdescr;   /* write descriptor          */
      char                   hf_fltdescr;       /* filter descriptor         */
      /* indices to filter[]  */
      char                   h_fltblkbeg;   /* begin filter block        */
      char                   h_fltblkend;       /* end filter block          */
      /* index to flt_descr[] */
      char                   h_nextdescr;       /* next descriptor           */
      char                   h_dummy;
      short                  i_descriptors;     /* number of descriptors     */
      };

#endif

#endif
