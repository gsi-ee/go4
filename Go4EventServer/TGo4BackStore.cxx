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

#include "TGo4BackStore.h"

#include "TTree.h"
#include "TDirectory.h"
#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4BackStoreParameter.h"
#include "TGo4EventElement.h"
#include "TGo4CompositeEvent.h"

const char* TGo4BackStore::fgcTREESUF = "xTree";
const char* TGo4BackStore::fgcEVBRANCHNAME = "Go4EventBranch.";

const Int_t TGo4BackStore::fgiMAXAUTOBYTES = 100000000;

TGo4BackStore::TGo4BackStore() :
   TGo4EventStore("Go4 Default Back Store"),
   fxTree(0),
   fbBranchExists(kFALSE),
   fxEvent(0),
   fiSplit(1),
   fiBufsize(0),
   fiFillCount(0)
{
   GO4TRACE((15,"TGo4BackStore::TGo4BackStore()", __LINE__, __FILE__));
   // public default ctor for streamer
}

TGo4BackStore::TGo4BackStore(TGo4BackStoreParameter* par) :
   TGo4EventStore("dummy"),
   fxTree(0),
   fbBranchExists(kFALSE),
   fxEvent(0),
   fiSplit(1),
   fiBufsize(0),
   fiFillCount(0)
{
   GO4TRACE((15,"TGo4BackStore::TGo4BackStore(TGo4BackStoreParameter* par)", __LINE__, __FILE__));

   if (par==0) {
      TGo4Log::Error("TGo4BackStoreParameter is not specified in TGo4BackStore constructor");
      return;
   }

   fiSplit = par->GetSplitlevel();
   fiBufsize = par->GetBufsize();

   SetName(par->GetName());
     // strip any path information from treename (could be identical with filename!)
   const char* lastname = par->GetTitle();
   const char* oldname = lastname;
   lastname=strstr(oldname,"/");
   while(lastname!=0)
      {
         oldname=lastname+1;
         lastname=strstr(oldname,"/");
      }
   TString buffer = oldname;
   buffer+=fgcTREESUF;
   TDirectory *dirsav = gDirectory;
   gROOT->cd();
   fxTree = new TTree(buffer.Data(), "Go4BackStore");
   fxTree->SetBit(TGo4Status::kGo4BackStoreReset);
   fxTree->SetAutoSave(TGo4BackStore::fgiMAXAUTOBYTES);
   TGo4Log::Debug(" Tree %s has been created in memory ",buffer.Data());
   dirsav->cd();
}

TGo4BackStore::~TGo4BackStore()
{
   GO4TRACE((15,"TGo4BackStore::~TGo4BackStore()", __LINE__, __FILE__));
   delete fxTree;

}

void TGo4BackStore::Reset(Bool_t onlyclearflag)
{
   if(fxTree!=0) {

      if(!onlyclearflag)
         {
         fxTree->Reset();
         fxTree->SetBit(TGo4Status::kGo4BackStoreReset);
         }
     else
         {
         fxTree->ResetBit(TGo4Status::kGo4BackStoreReset);
         }
   }
}

Int_t TGo4BackStore::Store(TGo4Parameter* cali)
{
   return 0;
}

Int_t TGo4BackStore::Store(TGo4Condition* conny)
{
   return 0;
}

Int_t TGo4BackStore::Store(TGo4Fitter* fitty)
{
   return 0;
}

Int_t TGo4BackStore::Store(TFolder* foldy)
{
   return 0;
}


Int_t TGo4BackStore::Store(TGo4EventElement* event)
{
   GO4TRACE((12,"TGo4BackStore::Store(TGo4EventElement*)", __LINE__, __FILE__));

   fxEvent=event; // address of next event into event pointer
   if(!fbBranchExists)
      {
         // first call of Store, create new branch
         //std::cout << "**********************Creating new branch!"<< std::endl;
         if(fxEvent)
            {
               TString topbranchname(fxEvent->GetName());
               topbranchname+=".";
               TBranch*   go4branch= fxTree->GetBranch(topbranchname.Data());
               if(go4branch)
                  {
                     // tree already had branch of our name, check it
                     TGo4Log::Debug(" BackStore: Found existing branch %s , continue filling ",topbranchname.Data() );
                     // here we might check the classname of the stored events inbranch
                     go4branch->SetAddress(&fxEvent);
                     fbBranchExists=kTRUE;

                  }
               else
                  {
                     // no such branch existing, create a new one
                     TBranch *topbranch=
                     fxTree->Branch(topbranchname.Data(), fxEvent->ClassName(), &fxEvent, fiBufsize, fiSplit);
                     TGo4Log::Debug(" BackStore: Created new branch %s ", topbranchname.Data());
                     fbBranchExists=kTRUE;
                     if (fxEvent->InheritsFrom(TGo4CompositeEvent::Class()))
                      dynamic_cast<TGo4CompositeEvent*>  (fxEvent)->makeBranch(topbranch);
                  } // if(go4branch)
            }
         else
            {
               // this is an error....
               return 1;
            }
      } // if(!fbEventBranchExists)
   else
      {
           // need not to create a branch, use existing one
      }
   // check if autosave threshold is reached, reset tree before autosave is performed:
   Double_t totbytes=fxTree->GetTotBytes();
   if(totbytes>TGo4BackStore::fgiMAXAUTOBYTES)
      {
          fxTree->Reset();
          TGo4Log::Debug(" BackStore: Tree %s was Reset after %f bytes ",
             fxTree->GetName(), totbytes);
      }
   fxTree->Fill();
   return 0;
}




