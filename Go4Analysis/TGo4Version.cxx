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

#include "TGo4Version.h"

#include <iostream>

const Int_t TGo4Version::fgiGO4VERSION= __GO4BUILDVERSION__;

TGo4Version* TGo4Version::fxInstance = nullptr;

TGo4Version* TGo4Version::Instance()
{
   if (!fxInstance)
     fxInstance = new TGo4Version();
   return fxInstance;
}

Int_t TGo4Version::GetBuildVersion()
{
   return fgiGO4VERSION;
}

TGo4Version::TGo4Version()
{
}

TGo4Version::~TGo4Version()
{
}

Bool_t TGo4Version::CheckVersion(Int_t version)
{
//
   if(fgiGO4VERSION==version) return kTRUE;

   std::cerr <<" !!!! VERSION MISMATCH: \n\tUser Analysis Build \tVersion " << fgiGO4VERSION << std::endl;
   std::cerr <<"\tdoes not match current \tVersion " << version << std:: endl;
   return kFALSE;
}
