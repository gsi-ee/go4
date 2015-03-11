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

#include <string.h>

#include "TFolder.h"
#include "THttpServer.h"
#include "TClass.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TDatime.h"
#include "TROOT.h"
#include "TH1F.h"

#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4Log.h"
#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4AnalysisWebStatus.h"
#include "TGo4EventElement.h"
#include "TGo4Ratemeter.h"

THttpServer* TGo4Sniffer::gHttpServer = 0;

Bool_t TGo4Sniffer::CreateEngine(const char* args)
{
   if (gHttpServer == 0) {
      gHttpServer = new THttpServer("");

      gHttpServer->SetSniffer(new TGo4Sniffer("go4_sniffer"));

      gHttpServer->SetTimer(0);  // we disable timer - go4 will call ProcessRequests method itself

      gHttpServer->AddLocation("go4sys/", TGo4Log::GO4SYS());
   }

   return gHttpServer->CreateEngine(args);
}


TGo4Sniffer::TGo4Sniffer(const char* name) :
   TRootSniffer(name,"dabc"),
   TGo4AnalysisSniffer(),
   fAnalysisStatus(0)
{
   SetReadOnly(kFALSE);
   SetScanGlobalDir(kFALSE);

   fAnalysisStatus = TGo4Analysis::Instance()->CreateWebStatus();

   fAnalysisStatus->SetName("Analysis");

   RegisterObject("/Status", fAnalysisStatus);
   SetItemField("/Status/Analysis", "_autoload", "/go4sys/html/go4.js");
   SetItemField("/Status/Analysis", "_icon", "/go4sys/icons/control.png");

   CreateItem("/Status/State", "Current analysis state");
   SetItemField("/Status/State","_icon","img_question");
   SetItemField("/Status/State","value","---");
   SetItemField("/Status/State","_status","GO4.DrawAnalysisStatus");
   SetItemField("/Status/State", "event_rate", "0");
   SetItemField("/Status/State", "aver_rate", "0");
   SetItemField("/Status/State", "run_time", "0");
   SetItemField("/Status/State", "event_count", "0");

   CreateItem("/Status/Message", "Last message from analysis");
   SetItemField("/Status/Message", "_kind","Text");
   SetItemField("/Status/Message", "value","---");

   CreateItem("/Status/DebugOutput", "Go4 debug output");
   SetItemField("/Status/DebugOutput", "_kind","Text");
   SetItemField("/Status/DebugOutput", "value","---");

   fEventRate = new TGraph();
   fEventRate->SetName("EventsRate");
   fEventRate->GetXaxis()->SetTimeDisplay(1);
   fEventRate->GetXaxis()->SetTimeFormat("%H:%M:%S");

   RegisterObject("/Status", fEventRate);

   RegisterCommand("/Status/CmdClear", "this->CmdClear()", "button;/go4sys/icons/clear.png");
   SetItemField("/Status/CmdClear", "_title", "Clear histograms and conditions in analysis");
   //SetItemField("/Status/CmdClear", "_hidden", "true");

   RegisterCommand("/Status/CmdStart", "this->CmdStart()", "button;/go4sys/icons/start.png");
   SetItemField("/Status/CmdStart", "_title", "Start analysis");
   //SetItemField("/Status/CmdStart", "_hidden", "true");

   RegisterCommand("/Status/CmdStop", "this->CmdStop()", "button;/go4sys/icons/Stop.png");
   SetItemField("/Status/CmdStop", "_title", "Stop analysis");
   //SetItemField("/Status/CmdStop", "_hidden", "true");

   RegisterCommand("/Status/CmdRestart", "this->CmdRestart()", "button;/go4sys/icons/restart.png");
   SetItemField("/Status/CmdRestart", "_title", "Resubmit analysis configuration and start again");
   //SetItemField("/Status/CmdRestart", "_hidden", "true");

   // set at the end when other items exists
   SetItemField("/", "_autoload", "/go4sys/html/go4.js");
   SetItemField("/", "_icon", "/go4sys/icons/go4logo2_small.png");

   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(this);

   TGo4Log::SetSniffer(this);
}

TGo4Sniffer::~TGo4Sniffer()
{
   if (TGo4Analysis::Instance()!=0)
      TGo4Analysis::Instance()->SetSniffer(0);

   TGo4Log::SetSniffer(0);
}

void TGo4Sniffer::ScanRoot(TRootSnifferScanRec& rec)
{
   TRootSniffer::ScanRoot(rec);

   TGo4AnalysisObjectManager* om(0);
   if (TGo4Analysis::Instance()) om = TGo4Analysis::Instance()->ObjectManager();

   if (om==0) return;

   //TGo4LockGuard mainlock; // JAM now done in top level invocation ProcessEvents

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
      rec.SetField("_autoload", "/go4sys/html/go4.js");
      rec.SetField("_drawfunc", "GO4.drawParameter");
      rec.SetField("_drawscript", "/go4sys/html/pareditor.js");
      rec.SetField("_drawopt", "editor");
      rec.SetField("_icon", "/go4sys/icons/parameter.png");
      return;
   }

   if (obj && obj->InheritsFrom(TGo4Condition::Class())) {
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
      TGo4Analysis::Instance()->UpdateStatus(fAnalysisStatus);
      if (cl) *cl = fAnalysisStatus->IsA();
      return fAnalysisStatus;
   }

   return TRootSniffer::FindInHierarchy(path, cl, member, chld);
}


Bool_t TGo4Sniffer::CmdStart()
{
   Info("CmdStart", "Resume analysis loop");

   TGo4Analysis* an = TGo4Analysis::Instance();
   if (an) {
      an->StartAnalysis();
      StatusMessage(0, "Resume analysis loop");
   }

   return kTRUE;
}

Bool_t TGo4Sniffer::CmdStop()
{
   Info("CmdStop", "Suspend analysis loop");
   TGo4Analysis* an = TGo4Analysis::Instance();
   if (an) {
      an->StopAnalysis();
      StatusMessage(0, "Suspend analysis loop");
   }
   return kTRUE;
}

Bool_t TGo4Sniffer::CmdClear()
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   if (an) {
      an->ClearObjects("Histograms");
      an->ClearObjects("Conditions");
      StatusMessage(0, "Clear Histograms and Conditions folder");
   }

   Info("CmdClear", "Clear Histograms and Conditions folder");
   return kTRUE;
}

Bool_t TGo4Sniffer::CmdRestart()
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   TGo4AnalysisClient* cli = an ? an->GetAnalysisClient() : 0;

   if (cli != 0) {
      cli->Stop();
      an->CloseAnalysis();
      an->InitEventClasses();
      cli->Start();
    }
    else if (an) {
      an->StopAnalysis();
      an->PostLoop();
      an->CloseAnalysis();
      an->InitEventClasses();
      an->PreLoop();
      an->StartAnalysis();
    }

   StatusMessage(0, "Restart analysis loop");
   Info("CmdRestart", "Restart analysis loop");

   return kTRUE;
}


void TGo4Sniffer::SetTitle(const char* title)
{
   // Method called by logger with every string, going to output

   if ((title==0) || (strlen(title)==0)) return;

   const char* prev = GetItemField("/Status/DebugOutput", "value");
   TString res;
   if (prev && (strcmp(prev,"---")!=0)) res = prev;
   res.Append("\n"); res.Append(title);

   SetItemField("/Status/DebugOutput","value", res);
}

void TGo4Sniffer::RatemeterUpdate(TGo4Ratemeter* r)
{
   TGo4Analysis* an = TGo4Analysis::Instance();
   Bool_t running = an ? an->IsRunning() : kFALSE;

   // Info("RatemeterUpdate" ," runnig %d", running);

   SetItemField("/Status/State","value", running ? "Running" : "Stopped");

   SetItemField("/Status/State", "event_rate", Form("%3.1f", r->GetRate()));
   SetItemField("/Status/State", "aver_rate", Form("%3.1f", r->GetAvRate()));
   SetItemField("/Status/State", "event_count", Form("%lu", (long unsigned) r->GetCurrentCount()));
   SetItemField("/Status/State", "run_time", Form("%3.1f", r->GetTime()));

   Int_t n = fEventRate->GetN();
   if (n==100) {
      fEventRate->RemovePoint(0);
      n--;
   }

   TDatime tm;
   fEventRate->SetPoint(n, tm.GetTime(), r->GetRate());
   fEventRate->GetXaxis()->SetTimeDisplay(1);
   fEventRate->GetXaxis()->SetTimeFormat("%H:%M:%S%F1995-01-01 00:00:00");
}

void TGo4Sniffer::StatusMessage(int level, const TString &msg)
{
   const char* prev = GetItemField("/Status/Message", "value");
   TString res;
   if (prev && (strcmp(prev,"---")!=0)) res = prev;
   res.Append("\n"); res.Append(msg);

   SetItemField("/Status/Message","value",res);
}

void TGo4Sniffer::ProcessSnifferEvents()
{
   // Method called from the thread, where go4 analysis executed

   if (gHttpServer) gHttpServer->ProcessRequests();
}

