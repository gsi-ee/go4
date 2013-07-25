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

#include "TGo4FileSourceParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4FileSourceParameter::TGo4FileSourceParameter(const char* name)
: TGo4EventSourceParameter(name, GO4EV_FILE)
{
  GO4TRACE((14,"TGo4FileSourceParameter::TGo4FileSourceParameter(const char*,...)", __LINE__, __FILE__));
}

TGo4FileSourceParameter::TGo4FileSourceParameter()
: TGo4EventSourceParameter("Default Go4 FileSource", GO4EV_FILE)
{
  GO4TRACE((14,"TGo4FileSourceParameter::TGo4FileSourceParameter()", __LINE__, __FILE__));
}


TGo4FileSourceParameter::~TGo4FileSourceParameter()
{
   GO4TRACE((14,"TGo4FileSourceParameter::~TGo4FileSourceParameter()", __LINE__, __FILE__));
}
