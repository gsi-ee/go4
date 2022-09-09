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

#include "TGo4MemberStatus.h"

TGo4MemberStatus::TGo4MemberStatus() :
   TGo4ObjectStatus()
{
}

TGo4MemberStatus::TGo4MemberStatus(const char *name, const char *datatypename) :
   TGo4ObjectStatus(nullptr, kFALSE)
{
   SetName(name);
   SetTitle(datatypename);
   fxObjectClass = datatypename;
}

TGo4MemberStatus::~TGo4MemberStatus()
{
}

void TGo4MemberStatus::Print(Option_t*) const
{
   TGo4ObjectStatus::Print();
   PrintLine("G-OOOO-> Datamember Status Class Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
}
