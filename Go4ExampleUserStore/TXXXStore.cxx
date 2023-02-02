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

#include "TXXXStore.h"

#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TBranch.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4UserStoreParameter.h"
#include "TGo4EventElement.h"
#include "TXXXEvent.h"


TXXXStore::TXXXStore() :
   TGo4EventStore("User custom store")
{
   GO4TRACE((15,"TXXXStore::TXXXStore()", __LINE__, __FILE__));
   // public default ctor for streamer
}

TXXXStore::TXXXStore(TGo4UserStoreParameter *par) :
   TGo4EventStore("User custom store")
{
   GO4TRACE((15,"TXXXStore::TXXXStore(TGo4UserStoreParameter* par)", __LINE__, __FILE__));

   if (!par) {
      TGo4Log::Error("TGo4UserStoreParameter is not specified in TXXXStore constructor");
      return;
   }

   SetName(par->GetName());

   TString fname = par->GetName();
   if (!fname.Contains(".root")) fname.Append(".root");

   fxFile = TFile::Open(fname.Data(), "RECREATE");
   TGo4Log::Info("TXXXStore: Open file %s RECREATE", fname.Data());

   fxTree = new TTree("Custom", "Custom go4 store");
   fxTree->Write();
}

TXXXStore::~TXXXStore()
{
   if (fxFile && fxTree) {
      fxFile = fxTree->GetCurrentFile(); // for file split after 1.8 Gb!
      fxFile->cd();
      fxTree->Write(nullptr, TObject::kOverwrite);
      delete fxFile; // closes File, fxTree is removed from memory then
      fxFile = nullptr;
      fxTree = nullptr;
   }
}

Int_t TXXXStore::Store(TGo4EventElement *event)
{
   GO4TRACE((12,"TXXXStore::Store(TGo4EventElement *)", __LINE__, __FILE__));

   TXXXEvent *custom = dynamic_cast<TXXXEvent *>(event); // address of next event into event pointer
   if (!custom) return 1; // error

   if(!fbBranchExists) {
      fxEvent = custom;
      fxTree->Branch("Crate1", fxEvent->fCrate1, "Create1[8]/F");
      fxTree->Branch("Crate2", fxEvent->fCrate2, "Create2[8]/F");
      fbBranchExists = kTRUE;
   } else if (fxEvent != custom) {
      TGo4Log::Info("TXXXStore: Event pointer changed");
      return 1; // error, should never happen
   }

   fxTree->Fill();
   return 0;
}
