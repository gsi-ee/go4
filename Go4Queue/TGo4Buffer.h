// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4BUFFER_HEADER
#define TGO4BUFFER_HEADER

#include "RVersion.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(5,15,1)

#include "TBuffer.h"
#define TGo4Buffer TBuffer

#else

#include "TBufferFile.h"
#define TGo4Buffer TBufferFile

#endif

#endif
