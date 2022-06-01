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

#include "TGo4MainTree.h"

#include "TTree.h"
#include "TFile.h"

#include "TGo4Log.h"

TGo4MainTree *TGo4MainTree::fxInstance = nullptr;

const char* TGo4MainTree::fgcTREENAME = "Main";
const char* TGo4MainTree::fgcFILENAME = "Go4MainTree.root";

const Int_t TGo4MainTree::fgiCOMPRESS = 5;
const Int_t TGo4MainTree::fgiAUTOSAVESIZE = 10000000;

TGo4MainTree::TGo4MainTree() :
   TObject(),
   fxTree(nullptr),
   fiMaxIndex(0),
   fiCurrentIndex(0)
{
   GO4TRACE((15,"TGo4MainTree::TGo4MainTree()", __LINE__, __FILE__));

   fxFile = TFile::Open(fgcFILENAME, "UPDATE", "File for main Go4 tree", fgiCOMPRESS);
   TGo4Log::Info("TGo4MainTree: Open file %s UPDATE", fgcFILENAME);
   // check if tree already exists...
   fxTree = dynamic_cast<TTree*> (fxFile->Get(fgcTREENAME));
   if(fxTree) {
      TGo4Log::Debug(" MainTree has been found in file %s ",fgcFILENAME);
   } else {
      fxTree = new TTree(fgcTREENAME, "The Go4 Tree");
      fxTree->SetAutoSave(fgiAUTOSAVESIZE);
      TGo4Log::Debug(" MainTree has been created in file %s ",fgcFILENAME);
   }
   fiMaxIndex = (Int_t) fxTree->GetEntries();
}

TGo4MainTree::~TGo4MainTree()
{
   GO4TRACE((15, "TGo4MainTree::~TGo4MainTree()", __LINE__, __FILE__));
   Write();
   delete fxFile;
}

TGo4MainTree * TGo4MainTree::Instance()
{
   GO4TRACE((12, "TGo4MainTree::Instance()", __LINE__, __FILE__));
   if (!fxInstance)
      fxInstance = new TGo4MainTree();
   return fxInstance;
}

void TGo4MainTree::SetAutoSave(Int_t bytesinterval)
{
   GO4TRACE((15,"TGo4MainTree::SetAutoSave(Int_t)", __LINE__, __FILE__));
   Instance();
   fxTree->SetAutoSave(bytesinterval);
}

Int_t TGo4MainTree::Write(const char* /*dummy*/, Int_t /*option*/, Int_t /*bufsize*/)
{
   GO4TRACE((12,"TGo4MainTree::Write()", __LINE__, __FILE__));
   fxFile->cd();
   fxTree->Write(0, TObject::kOverwrite);
   return 0;
}

Int_t TGo4MainTree::Write(const char* /*dummy*/, Int_t /*option*/, Int_t /*bufsize*/) const
{
   GO4TRACE((12,"TGo4MainTree::Write()", __LINE__, __FILE__));
   fxFile->cd();
   fxTree->Write(0, TObject::kOverwrite);
   return 0;
}

void TGo4MainTree::Update()
{
   GO4TRACE((12, "TGo4MainTree::Update()", __LINE__, __FILE__));
   if (GetCurrentIndex() >= GetMaxIndex()) {
      // if we are at the end of the tree, increment TTree fEvents counter
      // without affecting the branches:
      fxTree->SetBranchStatus("*", 0);
      fxTree->Fill();
      fxTree->SetBranchStatus("*", 1);
   }
   IncCurrentIndex();
}

Int_t TGo4MainTree::GetMaxIndex()
{
   fiMaxIndex = !fxTree ? 0 : fxTree->GetEntries();
   return fiMaxIndex;
}


