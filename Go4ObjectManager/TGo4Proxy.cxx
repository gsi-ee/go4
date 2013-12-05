// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4Proxy.h"

#include "TROOT.h"

#include "TGo4ObjectManager.h"


Bool_t TGo4Access::GetObject(TObject* &obj, Bool_t &owner) const
{
   obj = 0;
   owner = kFALSE;
   return kFALSE;
}


const char* TGo4Access::GetObjectName() const
{
   return 0;
}

TClass* TGo4Access::GetObjectClass() const
{
   const char* name = GetObjectClassName();
   return name==0 ? 0 : (TClass*) gROOT->GetListOfClasses()->FindObject(name);
}

const char* TGo4Access::GetObjectClassName() const
{
   return 0;
}

Int_t TGo4Access::AssignObjectTo(TGo4ObjectManager* rcv, const char* path)
{
   if ((rcv==0) || IsRemote()) return 0;
   TObject* obj = 0;
   Bool_t owner = kFALSE;
   if (!GetObject(obj, owner)) return 0;
   DoObjectAssignement(rcv, path, obj, owner);
   return 1;
}

void TGo4Access::DoObjectAssignement(TGo4ObjectManager* mgr,
                                     const char* path,
                                     TObject* obj,
                                     Bool_t owner)
{
   mgr->AssignObject(path, obj, owner);
}
