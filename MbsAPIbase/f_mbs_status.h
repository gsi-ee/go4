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

#ifndef F_MBS_STATUS_H
#define F_MBS_STATUS_H

#include "typedefs.h"
#include "s_daqst.h"
#include "s_setup.h"
#include "s_set_ml.h"
#include "s_set_mo.h"

INTS4 f_mbs_status(CHARS *c_node, s_daqst *ps_daqst);
INTS4 f_mbs_setup(CHARS *c_node, s_setup *ps_setup);
INTS4 f_mbs_ml_setup(CHARS *c_node, s_set_ml *ps_set_ml);
INTS4 f_mbs_mo_setup(CHARS *c_node, s_set_mo *ps_set_mo);

#endif
