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

#include "TGo4MbsRandomParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4MbsRandomParameter::TGo4MbsRandomParameter() :
   TGo4MbsSourceParameter("Default Go4 MbsRandom", GO4EV_MBS_RANDOM)
{
   GO4TRACE((14,"TGo4MbsRandomParameter::TGo4MbsRandomParameter()", __LINE__, __FILE__));
}

TGo4MbsRandomParameter::TGo4MbsRandomParameter(const char* name) :
   TGo4MbsSourceParameter(name, GO4EV_MBS_RANDOM)
{
   GO4TRACE((14,"TGo4MbsRandomParameter::TGo4MbsRandomParameter(onst char*,...)", __LINE__, __FILE__));
}

TGo4MbsRandomParameter::~TGo4MbsRandomParameter()
{
  GO4TRACE((14,"TGo4MbsRandomParameter::~TGo4MbsRandomParameter()", __LINE__, __FILE__));
}
