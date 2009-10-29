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

#include "TGo4MainTree.h"

#include "TTree.h"
#include "TFile.h"

#include "TGo4Log.h"
#include "Go4Exceptions.h"


TGo4MainTree * TGo4MainTree::fxInstance= 0;

const char* TGo4MainTree::fgcTREENAME = "Main";
const char* TGo4MainTree::fgcFILENAME = "Go4MainTree.root";

const Int_t TGo4MainTree::fgiCOMPRESS = 5;
const Int_t TGo4MainTree::fgiAUTOSAVESIZE = 10000000;

TGo4MainTree::TGo4MainTree() :
   TObject(),
   fxTree(0),
   fiMaxIndex(0),
   fiCurrentIndex(0)
{
   TRACE((15,"TGo4MainTree::TGo4MainTree()", __LINE__, __FILE__));

   fxFile = new TFile(fgcFILENAME, "UPDATE");
   TGo4Log::Info("TGo4MainTree: Open file %s UPDATE", fgcFILENAME);
   fxFile->SetCompressionLevel(fgiCOMPRESS);
   // check if tree already exists...
   fxTree= dynamic_cast<TTree*> (fxFile->Get(fgcTREENAME));
   if(fxTree)
      {
         TGo4Log::Debug(" MainTree has been found in file %s ",fgcFILENAME);
      }
   else
      {
         fxTree = new TTree(fgcTREENAME, "The Go4 Tree");
         fxTree->SetAutoSave(fgiAUTOSAVESIZE);
         TGo4Log::Debug(" MainTree has been created in file %s ",fgcFILENAME);
      }
    fiMaxIndex= (Int_t ) fxTree->GetEntries();

}

TGo4MainTree::~TGo4MainTree()
{
  TRACE((15,"TGo4MainTree::~TGo4MainTree()", __LINE__, __FILE__));
   Write();
   delete fxFile;

}


TGo4MainTree * TGo4MainTree::Instance()
{
   TRACE((12,"TGo4MainTree::Instance()", __LINE__, __FILE__));
      if (fxInstance == 0)
         {
            fxInstance = new TGo4MainTree();
         }
      else { }
      return fxInstance;
}

void TGo4MainTree::SetAutoSave(Int_t bytesinterval)
{
   TRACE((15,"TGo4MainTree::SetAutoSave(Int_t)", __LINE__, __FILE__));
   Instance();
   fxTree->SetAutoSave(bytesinterval);
}

Int_t TGo4MainTree::Write(const char* /*dummy*/, Int_t /*option*/, Int_t /*bufsize*/)
{
   TRACE((12,"TGo4MainTree::Write()", __LINE__, __FILE__));
   fxFile->cd();
   fxTree->Write(0, TObject::kOverwrite);
   return 0;
}

Int_t TGo4MainTree::Write(const char* /*dummy*/, Int_t /*option*/, Int_t /*bufsize*/) const
{
   TRACE((12,"TGo4MainTree::Write()", __LINE__, __FILE__));
   fxFile->cd();
   fxTree->Write(0, TObject::kOverwrite);
   return 0;
}

void TGo4MainTree::Update()
{
TRACE((12,"TGo4MainTree::Update()", __LINE__, __FILE__));
   if( GetCurrentIndex() >= GetMaxIndex() )
      {
         // if we are at the end of the tree, increment TTree fEvents counter
         // without affecting the branches:
         fxTree->SetBranchStatus("*",0);
         fxTree->Fill();
         fxTree->SetBranchStatus("*",1);
      }
   else
      { }
   IncCurrentIndex();
}

Int_t TGo4MainTree::GetMaxIndex()
{
   fiMaxIndex = (fxTree==0) ? 0 : fxTree->GetEntries();
   return fiMaxIndex;
}


