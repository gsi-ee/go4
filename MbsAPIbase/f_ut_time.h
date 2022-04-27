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

#ifndef F_UT_TIME_H
#define F_UT_TIME_H

#include "typedefs.h"

#include <time.h>

CHARS *f_ut_time (CHARS *pc_time);

#ifdef _MSC_VER

#define CLOCK_REALTIME 1
int clock_gettime(int clockid, struct timespec *tp);

#endif

#endif
