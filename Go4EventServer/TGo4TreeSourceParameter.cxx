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

#include "TGo4TreeSourceParameter.h"

#include "TGo4Log.h"
#include "Go4EventServerTypes.h"

TGo4TreeSourceParameter::TGo4TreeSourceParameter(const char* name)
: TGo4EventSourceParameter(name, GO4EV_TREE)
{
  TRACE((14,"TGo4TreeSourceParameter::TGo4TreeSourceParameter(const char*)", __LINE__, __FILE__));
}


TGo4TreeSourceParameter::TGo4TreeSourceParameter()
: TGo4EventSourceParameter("Default Go4 TreeSource", GO4EV_TREE)
{
  TRACE((14,"TGo4TreeSourceParameter::TGo4TreeSourceParameter()", __LINE__, __FILE__));
}

TGo4TreeSourceParameter::~TGo4TreeSourceParameter()
{
  TRACE((14,"TGo4TreeSourceParameter::~TGo4TreeSourceParameter()", __LINE__, __FILE__));
}
