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

#include "TGo4EventElement.h"

#include "TTree.h"

#include "TGo4Log.h"
#include "TGo4EventSource.h"

R__EXTERN TTree *gTree;

TGo4EventElement::TGo4EventElement() :
   TNamed("Go4Element","This is a Go4 EventElement"),
   fbIsValid(kTRUE),
   fxParent(0),
   fxEventSource(0),
   fIdentifier(-1),
   isActivated(kFALSE),
   fDebug(kFALSE)
{

   TRACE((15,"TGo4EventElement::TGo4EventElement()",__LINE__, __FILE__));
}

TGo4EventElement::TGo4EventElement(const char* name) :
   TNamed(name,"This is a Go4 EventElement"),
   fbIsValid(kTRUE),
   fxParent(0),
   fxEventSource(0),
   fIdentifier(-1),
   isActivated(kFALSE),
   fDebug(kFALSE)
{
   TRACE((15,"TGo4EventElement::TGo4EventElement(const char*)",__LINE__, __FILE__));
}

TGo4EventElement::TGo4EventElement(const char* aName, const char* aTitle, Short_t aBaseCat) :
   TNamed(aName,aTitle),
   fbIsValid(kTRUE),
   fxParent(0),
   fxEventSource(0),
   fIdentifier(aBaseCat),
   isActivated(kFALSE),
   fDebug(kFALSE)
{
}

TGo4EventElement::~TGo4EventElement()
{
   TRACE((15,"TGo4EventElement::~TGo4EventElement()",__LINE__, __FILE__));
}

Bool_t TGo4EventElement::CheckEventSource(const char* classname)
{
   TRACE((12,"TGo4EventElement::CheckEventSource(const char*)",__LINE__, __FILE__));
   if(fxEventSource==0) return kFALSE;
   return fxEventSource->InheritsFrom(classname);
}

void TGo4EventElement::PrintEvent()
{
   TRACE((12,"TGo4EventElement::PrintEvent()",__LINE__, __FILE__));

   TGo4Log::Debug( " EventElement printout: ");
   TGo4Log::Debug( "\tIsValid=%d ",fbIsValid);
   if(fxEventSource)
   {
      TGo4Log::Debug( "\tEventSource: %s of class %s",
            fxEventSource->GetName(),
            fxEventSource->ClassName() );
   }
   else
   {
      TGo4Log::Debug( "\tNO EventSource");
   }
}

void TGo4EventElement::Print(Option_t* option) const
{
   ((TGo4EventElement*)this) -> PrintEvent();
}

void TGo4EventElement::makeBranch(TBranch *parent)
{
   // method for recursive branching algorithm

}

Int_t  TGo4EventElement::activateBranch(TBranch *branch,Int_t splitLevel,Int_t init)
{
   TString cad=branch->GetName();
   if(!isActivated){
      TTree* tree = branch->GetTree();
      TGo4EventElement *dump = this;
      tree->SetBranchAddress(cad.Data(), &dump );

      tree->SetBranchStatus(cad.Data(), 1);
      cad+="*";
      tree->SetBranchStatus(cad.Data(), 1);
      isActivated=kTRUE;
   }

   branch->GetEntry(0);
   return 0;
}

void TGo4EventElement::deactivate()
{
   TString name = GetName();
   name+=".";
   gTree->SetBranchStatus(name.Data(), 0);
   name+="*";
   gTree->SetBranchStatus(name.Data(), 0);
   TGo4Log::Info("-I- Deactivating elements at location : %s", name.Data());
}

void TGo4EventElement::activate()
{
   TString name=GetName();
   name+=".";
   gTree->SetBranchStatus(name.Data(), 1);
   name+="*";
   gTree->SetBranchStatus(name.Data(), 1);
   TGo4Log::Info("-I- Activating elements at location : %s", name.Data());
}

void TGo4EventElement::Clear(Option_t *)
{
}

Int_t TGo4EventElement::Init()
{
   Int_t res(0);
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
   Clear();

   if (fxEventSource==0) { SetValid(kFALSE); return 1; }

   if (fxEventSource->BuildEvent(this)) {
      SetValid(kTRUE);
      return 0;
   }

   Int_t res = fxEventSource->GetEventStatus();

   SetValid(kFALSE);

   return res==0 ? 1 : res;
}
