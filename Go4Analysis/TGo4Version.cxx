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

#include "TGo4Version.h"

#include "go4iostream.h"

const Int_t TGo4Version::fgiGO4VERSION= __GO4BUILDVERSION__;

TGo4Version* TGo4Version::fxInstance = 0;

TGo4Version* TGo4Version::Instance()
{
   if (fxInstance == 0)
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

   cerr <<" !!!! VERSION MISMATCH: \n\tUser Analysis Build \tVersion ";
   cerr << fgiGO4VERSION << endl;
   cerr <<"\tdoes not match current \tVersion ";
   cerr << version <<" " << endl;
   return kFALSE;
}
