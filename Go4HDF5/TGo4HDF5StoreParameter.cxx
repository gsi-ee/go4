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

#include "TGo4HDF5StoreParameter.h"

#include "TGo4Status.h"
#include "Go4EventServerTypes.h"

TGo4HDF5StoreParameter::TGo4HDF5StoreParameter(const char* name,  Go4_H5_File_Flags  flags)
: TGo4EventStoreParameter(name, GO4EV_HDF5),
   fiFlags(flags)
{
   SetTitle(name);
}

TGo4HDF5StoreParameter::TGo4HDF5StoreParameter() :
   TGo4EventStoreParameter("Default Go4 HDF5 Store", GO4EV_HDF5)
{
   SetTitle("Go4HDF5Store");
}

TGo4HDF5StoreParameter::~TGo4HDF5StoreParameter()
{
}

void TGo4HDF5StoreParameter::Print(Option_t*) const
{
   TGo4EventStoreParameter::Print();
   TGo4Status::PrintLine("  File flags: \t%d", fiFlags);
}

Bool_t TGo4HDF5StoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   auto filepar = dynamic_cast<TGo4HDF5StoreParameter*>(rhs);
   if (!filepar) return kFALSE;
   if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
   SetHDF5Flags(filepar->GetHDF5Flags());
   return kTRUE;
}
