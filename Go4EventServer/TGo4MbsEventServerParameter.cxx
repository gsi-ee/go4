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

#include "TGo4MbsEventServerParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsEventServerParameter::TGo4MbsEventServerParameter() :
  TGo4MbsSourceParameter("default mbs eventserver", GO4EV_MBS_EVENTSERVER)
{
   GO4TRACE((14,"TGo4MbsEventServerParameter::TGo4MbsEventServerParameter()", __LINE__, __FILE__));
}

TGo4MbsEventServerParameter::TGo4MbsEventServerParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_EVENTSERVER)
{
  GO4TRACE((14,"TGo4MbsEventServerParameter::TGo4MbsEventServerParameter(const char*)", __LINE__, __FILE__));
}

TGo4MbsEventServerParameter::~TGo4MbsEventServerParameter()
{
   GO4TRACE((14,"TGo4MbsEventServerParameter::~TGo4MbsEventServerParameter()", __LINE__, __FILE__));
}
