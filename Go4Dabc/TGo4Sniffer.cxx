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

#include "TGo4Sniffer.h"

#include "TFolder.h"

#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4LockGuard.h"
#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4EventElement.h"
#include "TClass.h"
#include <string.h>

#include "dabc/Hierarchy.h"

TGo4Sniffer::TGo4Sniffer(const char* name) :
   TRootSniffer(name,"dabc"),
   fAnalysisStatus(0)
{
   SetReadOnly(kFALSE);
}

void TGo4Sniffer::ScanRoot(TRootSnifferScanRec& rec)
{

   TGo4AnalysisObjectManager* om(0);
   TGo4AnalysisClient* cli(0);
   if (TGo4Analysis::Instance()) {
      om = TGo4Analysis::Instance()->ObjectManager();
      cli = TGo4Analysis::Instance()->GetAnalysisClient();
   }

   if (om==0) {
      TRootSniffer::ScanRoot(rec);
      return;
   }

   rec.SetField(dabc::prop_kind, "GO4.Analysis");
   rec.SetField("_autoload", "/go4sys/html/go4.js");

   {
      TRootSnifferScanRec chld;
      if (chld.GoInside(rec, 0, "StreamerInfo"))
         chld.SetField(dabc::prop_kind, "ROOT.TStreamerInfoList");
   }

   TGo4LockGuard mainlock;

   TFolder* main = om->GetObjectFolder();

   TFolder* hist_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcHISTFOLDER));
   TFolder* par_fold  = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcPARAFOLDER));
   TFolder* cond_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcCONDFOLDER));
   TFolder* tree_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcTREEFOLDER));
   TFolder* canv_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcCANVFOLDER));
   TFolder* anal_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcANALYSISFOLDER));
   TFolder* even_fold = dynamic_cast<TFolder*> (anal_fold->FindObject(TGo4AnalysisObjectManager::fgcEVENTFOLDER));
   TFolder* user_fold = dynamic_cast<TFolder*> (main->FindObject(TGo4AnalysisObjectManager::fgcUSRFOLDER));

   ScanCollection(rec, hist_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcHISTFOLDER);
   ScanCollection(rec, par_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcPARAFOLDER);
   ScanCollection(rec, cond_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcCONDFOLDER);
   ScanCollection(rec, tree_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcTREEFOLDER);
   ScanCollection(rec, canv_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcCANVFOLDER);
   ScanCollection(rec, even_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcEVENTFOLDER);
   ScanCollection(rec, user_fold->GetListOfFolders(), TGo4AnalysisObjectManager::fgcUSRFOLDER);
}

void TGo4Sniffer::ScanObjectProperties(TRootSnifferScanRec &rec, TObject *obj)
{
   TRootSniffer::ScanObjectProperties(rec, obj);

   if (obj && obj->InheritsFrom(TGo4Parameter::Class())) {
      // rec.SetField("_more", "true");
      rec.SetField("_editor", "true");
      rec.SetField("_drawfunc", "GO4.drawParameter");
      rec.SetField("_autoload", "/go4sys/html/go4.js");
      rec.SetField("_icon", "/go4sys/icons/parameter.png");
      return;
   }

   if (obj && obj->InheritsFrom(TGo4Condition::Class())) {
      rec.SetField("_editor", "true");
      rec.SetField("_autoload", "/go4sys/html/go4.js");
      rec.SetField("_icon", "/go4sys/icons/condedit.png");
      return;
   }

   if (obj && obj->InheritsFrom(TGo4EventElement::Class())) {
      rec.SetField("_more", "true");
      rec.SetField("_icon", "/go4sys/icons/eventobj.png");
      return;
   }
}

void* TGo4Sniffer::FindInHierarchy(const char *path, TClass **cl, TDataMember **member, Int_t *chld)
{
   if ((path!=0) && (strcmp(path,"Status/Analysis")==0)) {
      if (fAnalysisStatus==0)
         fAnalysisStatus = TGo4Analysis::Instance()->CreateStatus();
      // TGo4Analysis::Instance()->UpdateStatus(fAnalysisStatus);
      if (cl) *cl = fAnalysisStatus->IsA();

      return fAnalysisStatus;
   }

   return TRootSniffer::FindInHierarchy(path, cl, member, chld);
}

