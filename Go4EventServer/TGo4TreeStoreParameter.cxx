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

#include "TGo4TreeStoreParameter.h"

#include "Go4EventServerTypes.h"
#include "TGo4Status.h"

TGo4TreeStoreParameter::TGo4TreeStoreParameter(const char* name,
                                                Int_t splitlevel,
                                                Int_t bufsize,
                                                const char *filename,
                                                Int_t compression)
: TGo4EventStoreParameter(name, GO4EV_TREE),
   fiSplit(splitlevel), fiBufsize(bufsize), fiCompression(compression)
{
   if(filename) fxBranchFile = filename;
}

TGo4TreeStoreParameter::TGo4TreeStoreParameter()
: TGo4EventStoreParameter("default go4 treestore", GO4EV_TREE)
{
}


TGo4TreeStoreParameter::~TGo4TreeStoreParameter()
{
}

void TGo4TreeStoreParameter::Print(Option_t*) const
{
   TGo4EventStoreParameter::Print();
   TGo4Status::PrintLine("  Split level: \t%d", fiSplit);
   TGo4Status::PrintLine("  Compression level: \t%d", fiCompression);
   TGo4Status::PrintLine("  Buffer size: \t%d", fiBufsize);
   if(!fxBranchFile.IsNull())
      TGo4Status::PrintLine("  Branch file name: \t%s", fxBranchFile.Data());
}

Bool_t TGo4TreeStoreParameter::UpdateFrom(TGo4Parameter* rhs)
{
   if(rhs && rhs->InheritsFrom(TGo4TreeStoreParameter::Class())) {
      auto treepar = dynamic_cast<TGo4TreeStoreParameter*>(rhs);
      if(!treepar) return kFALSE;
      if(!TGo4EventStoreParameter::UpdateFrom(rhs)) return kFALSE;
      fiSplit = treepar->fiSplit;
      fiBufsize = treepar->fiBufsize;
      fiCompression = treepar->fiCompression;
      return kTRUE;
   }
   return kFALSE;
}
