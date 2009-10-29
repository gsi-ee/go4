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

#include "TGo4EventFactory.h"

#include "TGo4Log.h"

TGo4EventFactory::TGo4EventFactory(const char* name)
: TNamed(name, "This is a Go4 Event Factory")
{
   TRACE((15,"TGo4EventFactory::TGo4EventFactory(const char*)",__LINE__, __FILE__));
}

TGo4EventFactory::~TGo4EventFactory()
{
   TRACE((15,"TGo4EventFactory::~TGo4EventFactory()",__LINE__, __FILE__));
}

TGo4EventFactory::TGo4EventFactory()
{
   TRACE((15,"TGo4EventFactory::~TGo4EventFactory()",__LINE__, __FILE__));
}
