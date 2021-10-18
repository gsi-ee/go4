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

#ifndef S_ERRSTAT_H
#define S_ERRSTAT_H

#include "typedefs.h"
/*  s_errstat.h
 *  ===========
 *  purpose        : structure for f_ut_error control
 *  date           : 22-Nov-1994
 *  author         : R.S. Mayer
 *
 *  update         : 12-Jan-1995: Message log flag /RSM
 *
 */

struct s_errstat {
   INTS4          if_verbose;
   INTS4          if_msglog;
   CHARS         c_taskname[16];
};

#endif
