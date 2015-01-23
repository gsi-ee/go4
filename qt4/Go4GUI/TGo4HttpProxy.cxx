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

#include "TGo4HttpProxy.h"

TGo4HttpProxy::TGo4HttpProxy() :
   TGo4ServerProxy(),
   fNodeName(),
   fxHierarchy(0),
   fxParentSlot(0)
{
}

TGo4HttpProxy::~TGo4HttpProxy()
{
   if (fxHierarchy!=0) {
      fxHierarchy = 0;
   }
}


Bool_t TGo4HttpProxy::Connect(const char* nodename)
{
   fNodeName = nodename;

   printf("Connect %s IsA %s \n", nodename, ClassName());

   return UpdateHierarchy(kTRUE);
}

Bool_t TGo4HttpProxy::UpdateHierarchy(Bool_t sync)
{
   return kTRUE;
}

void TGo4HttpProxy::Initialize(TGo4Slot* slot)
{
   fxParentSlot = slot;
}

void TGo4HttpProxy::Finalize(TGo4Slot* slot)
{
}

Bool_t TGo4HttpProxy::HasSublevels() const
{
   return kFALSE;
}

TGo4Access* TGo4HttpProxy::MakeProxy(const char* name)
{
   printf("Make TGo4Access\n");

   if (fxHierarchy == 0) return 0;

   return 0;

   //return new TGo4HttpAccess(fNodeName.Data(), name);
}

TGo4LevelIter* TGo4HttpProxy::MakeIter()
{
   if (fxHierarchy == 0) return 0;

//   printf("TGo4HttpProxy::MakeIter()\n");

   return 0;

   // return new TGo4HttpLevelIter(hierarchy);
}

void TGo4HttpProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
}

void TGo4HttpProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
}

void TGo4HttpProxy::Update(TGo4Slot* slot, Bool_t strong)
{
   if (strong) {
      printf("GO4 WANTS update HTTP hierarchy - do it\n");
      UpdateHierarchy(kFALSE);
   }
}

Bool_t TGo4HttpProxy::RefreshNamesList()
{
   return UpdateHierarchy(kFALSE);
}

