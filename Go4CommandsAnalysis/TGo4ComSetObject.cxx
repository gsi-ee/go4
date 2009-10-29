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

#include "TGo4ComSetObject.h"

#include "TFolder.h"
#include "TH1.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4RemoteCommand.h"
#include "TGo4AnalysisObjectResult.h"
#include "TGo4Parameter.h"
#include "TGo4ParameterStatus.h"
#include "TGo4Condition.h"
#include "TGo4DynamicEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4Picture.h"


TGo4ComSetObject::TGo4ComSetObject(const char* obname)
:TGo4AnalysisObjectCommand("ANSetObject","Set existing object to new values or create new object",obname),
fxObject(0),fxClient(0), fxAna(0),fxResult(0)
{
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetProtection(kGo4ComModeController);
}

TGo4ComSetObject::TGo4ComSetObject()
:TGo4AnalysisObjectCommand("ANSetObject","Set existing object to new values or create new object","mypara"),
fxObject(0),fxClient(0), fxAna(0),fxResult(0)
 {
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetProtection(kGo4ComModeController);
}

TGo4ComSetObject::~TGo4ComSetObject()
{
   //delete fxObject; // cleanup is done depending on ExeCom
   delete fxResult; //
}

void TGo4ComSetObject::Set(TGo4RemoteCommand* remcom)
{
   if(remcom==0) return;
   TGo4AnalysisObjectCommand::Set(remcom);
   TObject* ob=remcom->GetAggregate(); // we take over ownership
   if(ob && ob!=fxObject) {
      delete fxObject;
      fxObject=ob;
   }
}

Int_t TGo4ComSetObject::ExeCom()
{
   fxClient=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (fxClient!=0)
      {
         if(fxObject==0)
            {
               fxClient->SendStatusMessage(3, kTRUE,"SetObject - ERROR:  no source object specified for  %s",
                                                GetObjectName());
               return 0;
            } else {}
         SetObjectName(fxObject->GetName()); // override target name...
         TGo4Log::Debug(" %s : Setting object %s  ",
            GetName(), GetObjectName());
         fxAna = TGo4Analysis::Instance();
         fxResult = new TGo4AnalysisObjectResult(GetObjectName());
         Bool_t isdynentry = kFALSE;
         // evaluate object type here:
         if(ExeSetParStatus(dynamic_cast<TGo4ParameterStatus*>(fxObject))==0)
            ;
         else if(ExeSetPar(dynamic_cast<TGo4Parameter*>(fxObject))==0)
            ;
         else if(ExeSetCon(dynamic_cast<TGo4Condition*>(fxObject))==0)
            ;
         else if(ExeSetDyn(dynamic_cast<TGo4DynamicEntry*>(fxObject))==0)
            isdynentry = kTRUE;
         else if(ExeSetHis(dynamic_cast<TH1*>(fxObject))==0)
            ;
         else if(ExeSetPic(dynamic_cast<TGo4Picture*>(fxObject))==0)
            ;
         else
            ExeSetObj(fxObject);

         fxAna->UpdateNamesList();
         TGo4AnalysisObjectNames* state = fxAna->GetNamesList();
         fxResult->SetNamesList(state); // note: nameslist is not owned by result object!
         if(fxResult->Action()!=kGo4ActionError) {
             TString fullname;
//             if (isdynentry) {
//                fullname = "DynamicLists/Go4DynamicList/";
//                fullname += GetObjectName();
//             } else {
                TFolder* top = fxAna->GetObjectFolder();
                fullname = top->FindFullPathName(GetObjectName());
                fullname.Remove(0, 6); // remove //Go4/ top folder name
//             }

             fxResult->SetObjectFullName(fullname);
         }
         fxClient->SendStatus(fxResult);
      }
   else
      {
         TRACE((11,"TGo4ComSetObject::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}

Int_t TGo4ComSetObject::ExeSetParStatus(TGo4ParameterStatus* par)
{
   if (par==0) return -1;
   TString buf;
   if(fxAna->SetParameterStatus(GetObjectName(),par)) {
      buf = Form("Parameter %s was set to new values.", GetObjectName());
      fxClient->SendStatusMessage(1, kTRUE, buf.Data());
      fxResult->SetAction(kGo4ActionEdit);
   } else {
      buf = Form("SetObject - ERROR: failed to set parameter %s",GetObjectName());
      fxClient->SendStatusMessage(3, kTRUE, buf.Data());
      fxResult->SetAction(kGo4ActionError);
   }
   fxResult->SetMessage(buf.Data());
   delete fxObject;
   return 0;
}

Int_t TGo4ComSetObject::ExeSetPar(TGo4Parameter* par)
{
   if(par==0) return -1;
   TString buf;
   if(fxAna->SetParameter(GetObjectName(),par)) {
      buf = Form("Parameter %s was set to new values.", GetObjectName());
      fxClient->SendStatusMessage(1, kTRUE, buf.Data());
      fxResult->SetAction(kGo4ActionEdit);
   } else {
      buf = Form("SetObject - ERROR: failed to set parameter %s",GetObjectName());
      fxClient->SendStatusMessage(3, kTRUE, buf.Data());
      fxResult->SetAction(kGo4ActionError);
   }
   fxResult->SetMessage(buf.Data());
   delete fxObject;
   return 0;
}

Int_t TGo4ComSetObject::ExeSetCon(TGo4Condition* conny)
{
   if(conny==0) return -1;
   TString buf;
   if(fxAna->SetAnalysisCondition(GetObjectName(),conny, kFALSE)) {
      buf = Form("Condition %s was set to new values.", GetObjectName());
      fxClient->SendStatusMessage(1, kTRUE, buf.Data());
      fxResult->SetAction(kGo4ActionEdit);
   } else {
      buf = Form("SetCondition - ERROR: failed to set %s", GetObjectName());
      fxClient->SendStatusMessage(3, kTRUE, buf.Data());
      fxResult->SetAction(kGo4ActionError);
   }
   fxResult->SetMessage(buf.Data());
   delete fxObject;
   return 0;
}

Int_t TGo4ComSetObject::ExeSetHis(TH1* his)
{
   if(his==0) return -1;
   TString buf;
   if(fxAna->AddHistogram(his)) {
      his->SetBit(TGo4Status::kGo4CanDelete); // dynamic objects may be deleted from gui
      fxResult->SetAction(kGo4ActionPlot);
      buf = Form("Added new histogram %s to Go4 folders.", GetObjectName());
      fxClient->SendStatusMessage(1, kFALSE, buf.Data());
   } else {
      buf = Form("ERROR on adding new histogram %s ", GetObjectName());
      fxClient->SendStatusMessage(3, kFALSE, buf.Data());
      fxResult->SetAction(kGo4ActionError);
      delete his;
   }
   fxResult->SetMessage(buf.Data());
   return 0;
}

Int_t TGo4ComSetObject::ExeSetDyn(TGo4DynamicEntry* dyn)
{
   if(dyn==0) return -1;
   TString buf;
   if(fxAna->AddDynamicEntry((TGo4DynamicEntry*)dyn->Clone())) {
      TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (dyn);
      if ((tentry!=0) && tentry->IsEnabledProcessing())
         fxAna->SetDynListInterval(tentry->GetDynListInterval());
      fxResult->SetAction(kGo4ActionEdit);
      buf = Form("Set new status for entry  %s of dynamic list %s.", GetObjectName(), GetFolderName());
      fxClient->SendStatusMessage(1, kTRUE, buf.Data());
   } else {
      buf = Form("Could not set status for entry %s of dynamic list %s !!!", GetObjectName(), GetFolderName());
      fxResult->SetAction(kGo4ActionError);
      fxClient->SendStatusMessage(2, kTRUE, buf.Data());
   }
   fxResult->SetMessage(buf.Data());
   delete fxObject;
   return 0;
}

Int_t TGo4ComSetObject::ExeSetPic(TGo4Picture* pic)
{
   if(pic==0) return -1;
   TString buf;
   if(fxAna->SetPicture(GetObjectName(),pic)) {
      fxResult->SetAction(kGo4ActionPlot);
      buf = Form("Picture %s was set to new values.", GetObjectName());
      fxClient->SendStatusMessage(1, kFALSE, buf.Data());
   } else {
      buf = Form("SetPicture - ERROR: failed to set %s",GetObjectName());
      fxClient->SendStatusMessage(3, kFALSE, buf.Data());
      fxResult->SetAction(kGo4ActionError);
   }
   fxResult->SetMessage(buf.Data());
   delete fxObject;
   return 0;
}


Int_t TGo4ComSetObject::ExeSetObj(TObject* ob)
{
   TString buf;
   if(fxAna->AddObject(dynamic_cast<TNamed*>(ob))) {
      fxResult->SetAction(kGo4ActionRefresh);
      buf = Form("Added new object %s to Go4 folders.", GetObjectName());
      fxClient->SendStatusMessage(1, kFALSE, buf.Data());
   } else {
      fxResult->SetAction(kGo4ActionError);
      buf = Form("ERROR on adding new object %s ", GetObjectName());
      fxClient->SendStatusMessage(3, kFALSE, buf.Data());
      delete ob;
   }
   fxResult->SetMessage(buf.Data());
   return 0;
}
