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

#include "TGo4EventElement.h"

#include <iostream>

#include "TTree.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TBranch.h"

#include "TGo4Log.h"
#include "TGo4EventSource.h"

TGo4EventElement::TGo4EventElement() :
   TNamed("Go4Element","This is a Go4 EventElement")
{
   GO4TRACE((15,"TGo4EventElement::TGo4EventElement()",__LINE__, __FILE__));
}

TGo4EventElement::TGo4EventElement(const char *name) :
   TNamed(name,"This is a Go4 EventElement")
{
   GO4TRACE((15,"TGo4EventElement::TGo4EventElement(const char*)",__LINE__, __FILE__));
}

TGo4EventElement::TGo4EventElement(const char *aName, const char *aTitle, Short_t aBaseCat) :
   TNamed(aName,aTitle),
   fIdentifier(aBaseCat)
{
}

TGo4EventElement::~TGo4EventElement()
{
   GO4TRACE((15,"TGo4EventElement::~TGo4EventElement()",__LINE__, __FILE__));
}

Bool_t TGo4EventElement::CheckEventSource(const char *classname)
{
   GO4TRACE((12,"TGo4EventElement::CheckEventSource(const char*)",__LINE__, __FILE__));
   return fxEventSource ? fxEventSource->InheritsFrom(classname) : kFALSE;
}

void TGo4EventElement::PrintEvent()
{
   GO4TRACE((12,"TGo4EventElement::PrintEvent()",__LINE__, __FILE__));

   TGo4Log::Info("EventElement printout: Name=%s IsValid=%s ", GetName(), fbIsValid ? "true" : "false");
   if(fxEventSource)
      TGo4Log::Info( "EventSource: %s of class %s", fxEventSource->GetName(), fxEventSource->ClassName() );
   else
      TGo4Log::Info( "NO EventSource");
}

void TGo4EventElement::Print(Option_t* option) const
{
   ((TGo4EventElement*)this)->PrintEvent();
}

TGo4EventElement* TGo4EventElement::GetChild(const char *name)
{
   if (!name || (strlen(name) == 0)) return this;

   if (strcmp(name,".") == 0) return this;

   if (strcmp(name,"..") == 0) return GetParent();

   return nullptr;
}


void TGo4EventElement::synchronizeWithTree(TTree *tree, TGo4EventElement** var_ptr)
{
   if (!tree) return;

   TBranch* topb = nullptr;
   TString searchname = GetName();
   if (searchname.Length()>0) {
      searchname += ".";
      topb = tree->FindBranch(searchname.Data());
   }

   // if no appropriate branches found, use first branch for the event
   // TODO: should we check class name of the branch?
   if (!topb) topb = (TBranch*) tree->GetListOfBranches()->First();

   Int_t index = tree->GetListOfBranches()->IndexOf(topb);

   // FIXME SL: should we activate other branches, reading not working when all branches disabled in the beginning
   // note: only deactivate subleafs _after_ address of top branch is set!
   // tree->SetBranchStatus("*",0);

   activateBranch(topb, index, var_ptr);
}

Int_t TGo4EventElement::activateBranch(TBranch *branch, Int_t init, TGo4EventElement** var_ptr)
{
   if (!branch) return 0;

   TString cad = branch->GetName();

   TTree* tree = branch->GetTree();

   if (var_ptr) {
      // SL 23.08.2013 - seems to be, at some point TTree::SetBranchAddress() signature was changed
      // now one other need to specify pointer on actual class like
      // UserEvent* ev = (UserEvent*) GetInputEvent();
      // tree->SetBranchAddress("branch.", &ev);
      // or one need explicitly specify which class we want to set to the branch
      // No idea when it happens, but newest 5.34 ROOT no longer working correctly.

      TClass *cl = *var_ptr ? (*var_ptr)->IsA() : nullptr;
      tree->SetBranchAddress(cad.Data(), var_ptr, nullptr, cl, kOther_t, true);
      //tree->SetBranchAddress(cad.Data(), (void**) var_ptr);
   }

   tree->SetBranchStatus(cad.Data(), 1);
   cad+="*";
   tree->SetBranchStatus(cad.Data(), 1);

   return 0;
}

void TGo4EventElement::deactivate()
{
   TGo4Log::Warn("-W- Could not deactivate() event element %s in this ROOT Version, do not use!", GetName());
}

void TGo4EventElement::activate()
{
   TGo4Log::Warn("-W- Could not activate() element %s in this ROOT Version, do not use!", GetName());
}

void TGo4EventElement::Clear(Option_t *)
{
}

Int_t TGo4EventElement::Init()
{
   Int_t res = 0;
   Clear();
   SetValid(kTRUE);
   if (fxEventSource) {
      TGo4Log::Info("Event %s has source %s class: %s", GetName(), fxEventSource->GetName(), fxEventSource->ClassName());
      if (!fxEventSource->CheckEventClass(IsA())) {
         TGo4Log::Error("Event %s, mismatch between event source and event class", GetName());
         res = 1;
      }
   } else {
      TGo4Log::Error("Event %s has no data source", GetName());
      res = 1;
   }
   return res;
}

Int_t TGo4EventElement::Fill()
{
   if (!fbKeepContents) Clear();
   fbKeepContents = kFALSE;
   if (!fxEventSource) { SetValid(kFALSE); return 1; }

   if (fxEventSource->BuildEvent(this)) {
      //SetValid(kTRUE); // JAM: do not override event validity as specified by user
      return 0;
   }

   Int_t res = fxEventSource->GetEventStatus();

   SetValid(kFALSE);

   return res == 0 ? 1 : res;
}


TTree* TGo4EventElement::CreateSampleTree(TGo4EventElement** sample)
{
   // create sample tree with event element as entry
   // to be able use such tree later, one should provide 'sample' pointer to keep
   // event instance associated with tree branch

   TDirectory* filsav = gDirectory;
   gROOT->cd();
   if (sample) {
      delete *sample;
      *sample = nullptr;
   }
   TGo4EventElement* clone = (TGo4EventElement*) Clone();
   TTree* thetree = new TTree(clone->GetName(), "Single Event Tree");
   thetree->SetDirectory(nullptr);
   if (sample) *sample = clone;
   thetree->Branch("Go4EventSample", clone->ClassName(), sample ? sample : &clone, 64000, 99);
   thetree->Fill();
   filsav->cd();
   if (!sample) delete clone;
   return thetree;
}

void TGo4EventElement::ShowSampleTree()
{
   TGo4EventElement *sample = nullptr;

   TTree* tr = CreateSampleTree(&sample);

   if (tr) tr->Show(0);
   std::cout << std::endl;
   delete tr;
   delete sample;
}

