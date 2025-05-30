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

#include "TGo4Sniffer.h"

#include <cstring>

#include "TClass.h"
#include "TFolder.h"
#include "THttpServer.h"
#include "TBufferJSON.h"
#include "TGraph.h"
#include "TDatime.h"
#include "TTimeStamp.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TFile.h"
#include "TMethodCall.h"
#include "TGo4HistogramStatus.h"

#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisObjectManager.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4Log.h"
#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4AnalysisWebStatus.h"
#include "TGo4EventElement.h"
#include "TGo4Ratemeter.h"
#include "TGo4DynamicEntry.h"
#include "TGo4MsgList.h"

THttpServer *TGo4Sniffer::gHttpServer = nullptr;

THttpServer *TGo4Sniffer::GetHttpServer()
{
   return gHttpServer;
}



Bool_t TGo4Sniffer::CreateEngine(const char *args)
{
   if (!gHttpServer) {
      gHttpServer = new THttpServer("");

      gHttpServer->SetSniffer(new TGo4Sniffer("go4_sniffer"));

      gHttpServer->SetTimer(0);  // we disable timer - go4 will call ProcessRequests method itself

      gHttpServer->AddLocation("go4sys/", TGo4Log::GO4SYS());

      TBufferJSON::SetFloatFormat("%15.9e");
   }

   if (strstr(args, "jsroot:") == args) {
      gHttpServer->SetJSROOT(args+7);
      return kTRUE;
   }

   return gHttpServer->CreateEngine(args);
}


TGo4Sniffer::TGo4Sniffer(const char *name) :
      TRootSnifferFull(name),
   TGo4AnalysisSniffer(),
   fDebugOutput("Log","Analysis log messages", 1000),
   fStatusMessages("Msg","Analysis status messages", 100),
   fbPythonBound(kFALSE)
{
   SetReadOnly(kFALSE);
   SetScanGlobalDir(kFALSE);

   #if ROOT_VERSION_CODE >= ROOT_VERSION(6,33,0)
   fAutoLoadArg = "go4sys/html5/go4.mjs";
   #else
   fAutoLoadArg = "jq;go4sys/html/go4.js";
   #endif

   SetAutoLoadGo4(fAutoLoadArg);

   fAnalysisStatus = TGo4Analysis::Instance()->CreateWebStatus();

   fAnalysisStatus->SetName("Analysis");

   RegisterObject("/Status", &fDebugOutput);
   RegisterObject("/Status", &fStatusMessages);

   if (!HasProduceMultiMethod()) {
      // new multi.json method was implemented together with requests pre and post processing
      SetItemField("/Status/Log", "_hidden", "true");
      SetItemField("/Status/Msg", "_hidden", "true");

      CreateItem("/Status/Message", "Last message from analysis");
      SetItemField("/Status/Message", "_kind","Text");
      SetItemField("/Status/Message", "value","---");

      CreateItem("/Status/DebugOutput", "Go4 debug output");
      SetItemField("/Status/DebugOutput", "_kind","Text");
      SetItemField("/Status/DebugOutput", "value","---");
   }

   fRatemeter = new TGo4Ratemeter();
   fRatemeter->SetName("Ratemeter");
   fRatemeter->SetTitle("Analysis rates");

   fEventRate = new TGraph();
   fEventRate->SetName("EventsRate");
   fEventRate->SetTitle("Events processing rate");
   fEventRate->GetXaxis()->SetTimeDisplay(1);
   fEventRate->GetXaxis()->SetTimeFormat("%H:%M:%S");
   fEventRate->GetYaxis()->SetTitle("Events/s");
   fEventRate->GetHistogram()->SetDirectory(nullptr);

   RegisterObject("/Status", fEventRate);

   RegisterObject("/Status", fRatemeter);
   SetItemField("/Status/Ratemeter", "_hidden", "true");
   SetItemField("/Status/Ratemeter", "_status", "GO4.DrawAnalysisRatemeter");

   RegisterCommand("/Control/CmdClear", "this->CmdClear();", "button;go4sys/icons/clear.png");
   SetItemField("/Control/CmdClear", "_title", "Clear histograms and conditions in analysis");
   SetItemField("/Control/CmdClear", "_hidden", "true");

   RegisterCommand("/Control/CmdStart", "this->CmdStart();", "button;go4sys/icons/start.png");
   SetItemField("/Control/CmdStart", "_title", "Start analysis");
   SetItemField("/Control/CmdStart", "_hidden", "true");

   RegisterCommand("/Control/CmdStop", "this->CmdStop();", "button;go4sys/icons/Stop.png");
   SetItemField("/Control/CmdStop", "_title", "Stop analysis");
   SetItemField("/Control/CmdStop", "_hidden", "true");

   RegisterCommand("/Control/CmdClose", "this->CmdClose();", "");
   SetItemField("/Control/CmdClose", "_title", "Close analysis");
   SetItemField("/Control/CmdClose", "_hidden", "true");

   RegisterCommand("/Control/CmdRestart", "this->CmdRestart();", "button;go4sys/icons/restart.png");
   SetItemField("/Control/CmdRestart", "_title", "Resubmit analysis configuration and start again");
   SetItemField("/Control/CmdRestart", "_hidden", "true");

   RegisterCommand("/Control/CmdExit", "this->CmdExit();", "");
   SetItemField("/Control/CmdExit", "_title", "Exit analysis process");
   SetItemField("/Control/CmdExit", "_hidden", "true");

   if (HasRestrictMethod()) {
      // together with Restrict method support of
      // commands with arguments was introduced

      RegisterCommand("/Control/CmdOpenFile", "this->CmdOpenFile(\"%arg1%\");", "button;go4sys/icons/fileopen.png");
      SetItemField("/Control/CmdOpenFile", "_title", "Open ROOT file in analysis");
      SetItemField("/Control/CmdOpenFile", "_hreload", "true"); // after execution hierarchy will be reloaded
      //SetItemField("/Control/CmdOpenFile", "_hidden", "true");

      RegisterCommand("/Control/CmdCloseFiles", "this->CmdCloseFiles();", "go4sys/icons/fileclose.png");
      SetItemField("/Control/CmdCloseFiles", "_title", "Close all opened files");
      SetItemField("/Control/CmdCloseFiles", "_hreload", "true"); // after execution hierarchy will be reloaded
      //SetItemField("/Control/CmdCloseFiles", "_hidden", "true");

      RegisterCommand("/Control/CmdClearObject", "this->CmdClearObject(\"%arg1%\");", "");
      SetItemField("/Control/CmdClearObject", "_title", "Clear object content");
      SetItemField("/Control/CmdClearObject", "_hidden", "true");

      RegisterCommand("/Control/CmdDeleteObject", "this->CmdDeleteObject(\"%arg1%\");", "");
      SetItemField("/Control/CmdDeleteObject", "_title", "Delete object from analysis");
      SetItemField("/Control/CmdDeleteObject", "_hidden", "true");

      RegisterCommand("/Control/CmdExecute", "this->CmdExecute(\"%arg1%\");", "go4sys/icons/macro_t.png");
      SetItemField("/Control/CmdExecute", "_title", "Execute interpreter line in the analysis context. '@' means 'TGo4Analysis::Instance()->' ; A leading '$' invokes python skript.");
      //SetItemField("/Control/CmdExecute", "_hidden", "true");
   }

   RegisterObject("/Control", fAnalysisStatus);
   if (!HasAutoLoadMethod())
      SetItemField("/Control/Analysis", "_autoload", fAutoLoadArg);
   SetItemField("/Control/Analysis", "_icon", "go4sys/icons/control.png");
   SetItemField("/Control/Analysis", "_not_monitor", "true");

   RegisterObject("/Control", this);
   SetItemField("/Control/go4_sniffer", "_hidden", "true");

   CreateItem("/Control/Terminal", "Analysis terminal");
   SetItemField("/Control/Terminal", "_icon", "go4sys/icons/analysiswin.png");
   SetItemField("/Control/Terminal", "_player", "GO4.drawAnalysisTerminal");

   RestrictGo4("/Control","visible=controller,admin");

   RestrictGo4("/Control/CmdExit","visible=admin");

   RestrictGo4("/Conditions", "allow=controller,admin");

   RestrictGo4("/Parameters", "allow=controller,admin&allow_method=CreateStatus");

   // set at the end when other items exists
   if (!HasAutoLoadMethod())
      SetItemField("/", "_autoload", fAutoLoadArg);
   SetItemField("/", "_icon", "go4sys/icons/go4logo2_small.png");
   SetItemField("/", "_title", "GO4 analysis");
   SetItemField("/", "_analysis_name", TGo4Analysis::Instance()->GetName());
   if (HasRestrictMethod())
      SetItemField("/", "_has_restrict", "true");

   if (HasProduceMultiMethod())
      SetItemField("/", "_has_produce_multi", "true");

   if (TGo4Analysis::Instance())
      TGo4Analysis::Instance()->SetSniffer(this);

   TGo4Log::SetSniffer(this);

   if (!HasRestrictMethod()) {
      // this was problem with original ROOT sniffer, it does not construct classes
      // and class required in exe.bin
      gROOT->GetClass("TGo4ParameterStatus", kTRUE, kTRUE);
      gROOT->GetClass("TGo4EventElement", kTRUE, kTRUE);
      gROOT->GetClass("TGo4CompositeEvent", kTRUE, kTRUE);
      gROOT->GetClass("TGo4AnalysisStatus", kTRUE, kTRUE);
      gROOT->GetClass("TGo4AnalysisWebStatus", kTRUE, kTRUE);
   }
}

TGo4Sniffer::~TGo4Sniffer()
{
   if (TGo4Analysis::Instance())
      TGo4Analysis::Instance()->SetSniffer(nullptr);

   TGo4Log::SetSniffer(nullptr);
}

void TGo4Sniffer::ScanRoot(TRootSnifferScanRec& rec)
{
   rec.SetField("_toptitle", "Go4 http server");

   TRootSnifferFull::ScanRoot(rec);

   TGo4AnalysisObjectManager *om = TGo4Analysis::Instance() ? TGo4Analysis::Instance()->ObjectManager() : nullptr;

   if (!om) return;

   //TGo4LockGuard mainlock; // JAM now done in top level invocation ProcessEvents

   TFolder *main = om->GetObjectFolder();

   TFolder *hist_fold = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetHISTFOLDER()));
   TFolder *par_fold  = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetPARAFOLDER()));
   TFolder *cond_fold = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetCONDFOLDER()));
   TFolder *pic_fold  = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetPICTFOLDER()));
   TFolder *tree_fold = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetTREEFOLDER()));
   TFolder *canv_fold = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetCANVFOLDER()));
   TFolder *anal_fold = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetANALYSISFOLDER()));
   TFolder *even_fold = dynamic_cast<TFolder *>(anal_fold->FindObject(TGo4AnalysisObjectManager::GetEVENTFOLDER()));
   TFolder *user_fold = dynamic_cast<TFolder *>(main->FindObject(TGo4AnalysisObjectManager::GetUSRFOLDER()));
   TFolder *files_fold = dynamic_cast<TFolder *>(main->FindObject("Files"));

   ScanCollection(rec, hist_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetHISTFOLDER());
   ScanCollection(rec, par_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetPARAFOLDER());
   ScanCollection(rec, cond_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetCONDFOLDER());
   ScanCollection(rec, pic_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetPICTFOLDER());
   ScanCollection(rec, tree_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetTREEFOLDER());
   ScanCollection(rec, canv_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetCANVFOLDER());
   ScanCollection(rec, even_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetEVENTFOLDER());
   ScanCollection(rec, user_fold->GetListOfFolders(), TGo4AnalysisObjectManager::GetUSRFOLDER());
   if (files_fold)
      ScanCollection(rec, files_fold->GetListOfFolders(), "Files");
}

void TGo4Sniffer::ScanObjectProperties(TRootSnifferScanRec &rec, TObject *obj)
{
   TRootSnifferFull::ScanObjectProperties(rec, obj);
   if (!obj)
      return;

   if (obj->TestBit(TGo4Status::kGo4CanDelete)) {
      rec.SetField("_can_delete", "true");
   }

   if (obj->TestBit(TGo4Status::kGo4NoReset)) {
      rec.SetField("_no_reset", "true");
   }

   if (obj->InheritsFrom(TGo4Parameter::Class())) {
      if (!HasAutoLoadMethod())
         rec.SetField("_autoload", fAutoLoadArg);
      rec.SetField("_drawfunc", "GO4.drawParameter");
   #if ROOT_VERSION_CODE >= ROOT_VERSION(6,33,0)
      rec.SetField("_drawscript", "modules:go4sys/html5/pareditor.mjs");
   #else
      rec.SetField("_drawscript", "go4sys/html/pareditor.js");
   #endif
      rec.SetField("_drawopt", "editor");
      rec.SetField("_icon", "go4sys/icons/parameter.png");
      return;
   }

   if (obj->InheritsFrom(TGo4Condition::Class())) {
      if (!HasAutoLoadMethod())
         rec.SetField("_autoload", fAutoLoadArg);
      rec.SetField("_icon", "go4sys/icons/condedit.png");
      return;
   }

   if (obj->InheritsFrom(TGo4EventElement::Class())) {
      rec.SetField("_more", "true");
      rec.SetField("_go4event", "true");
      rec.SetField("_icon", "go4sys/icons/eventobj.png");
      return;
   }

   if (obj->InheritsFrom(TGo4MsgList::Class())) {
      if (!HasAutoLoadMethod())
         rec.SetField("_autoload", fAutoLoadArg);
      rec.SetField("_make_request", "GO4.MakeMsgListRequest");
      rec.SetField("_after_request", "GO4.AfterMsgListRequest");
      rec.SetField("_icon", "img_text");
      return;
   }

}

void *TGo4Sniffer::FindInHierarchy(const char *path, TClass **cl, TDataMember **member, Int_t *chld)
{
   if (path && (strcmp(path,"Status/Analysis") == 0)) {
      TGo4Analysis::Instance()->UpdateStatus(fAnalysisStatus);
      if (cl) *cl = fAnalysisStatus->IsA();
      return fAnalysisStatus;
   }

   return TRootSnifferFull::FindInHierarchy(path, cl, member, chld);
}


Bool_t TGo4Sniffer::CmdStart()
{
   auto an = TGo4Analysis::Instance();
   if (an) {
      an->StartAnalysis();
      StatusMessage(0, kTRUE, "Resume analysis loop");
   }

   return kTRUE;
}

Bool_t TGo4Sniffer::CmdStop()
{
   auto an = TGo4Analysis::Instance();
   if (an) {
      an->StopAnalysis();
      StatusMessage(0, kTRUE, "Suspend analysis loop");
   }

   return kTRUE;
}

Bool_t TGo4Sniffer::CmdOpenFile(const char *fname)
{
   Info("CmdOpenFile", "Open ROOT file %s", fname);

   TGo4AnalysisObjectManager *om = TGo4Analysis::Instance() ? TGo4Analysis::Instance()->ObjectManager() : nullptr;

   if (om) {
      TFolder *main = om->GetObjectFolder();

      TFolder *files_fold = dynamic_cast<TFolder *>(main->FindObject("Files"));
      if (!files_fold) {
         files_fold = main->AddFolder("Files","ROOT files");
         files_fold->SetOwner(kTRUE);
      }

      auto f = dynamic_cast<TFile *> (files_fold->FindObject(fname));
      if (f) { files_fold->Remove(f); delete f; }

      f = TFile::Open(fname);
      if (!f) return kFALSE;

      files_fold->Add(f);
   }

   return kTRUE;
}

Bool_t TGo4Sniffer::CmdCloseFiles()
{
   Info("CmdCloseFiles", "Close all opened files");
   TGo4AnalysisObjectManager *om = TGo4Analysis::Instance() ? TGo4Analysis::Instance()->ObjectManager() : nullptr;
   if (om) {
      TFolder *main = om->GetObjectFolder();
      TFolder *files_fold = dynamic_cast<TFolder *>(main->FindObject("Files"));
      if (files_fold) {
         files_fold->Clear();
         main->Remove(files_fold);
         delete files_fold;
      }
   }
   return kTRUE;
}


Bool_t TGo4Sniffer::CmdClear()
{
   auto an = TGo4Analysis::Instance();
   if (an) {
      an->ClearObjects("Histograms");
      an->ClearObjects("Conditions");
      an->SendMessageToGUI(1, kTRUE, "Clear Histograms and Conditions folder");
   }
   return kTRUE;
}

Bool_t TGo4Sniffer::CmdRestart()
{
   auto an = TGo4Analysis::Instance();
   TGo4AnalysisClient *cli = an ? an->GetAnalysisClient() : nullptr;

   if (cli) {
      cli->Stop();
      an->CloseAnalysis();
      if (an->InitEventClasses())
         cli->Start();
    } else if (an) {
      an->StopAnalysis();
      an->PostLoop();
      an->CloseAnalysis();
      if (an->InitEventClasses()) {
         an->PreLoop();
         an->StartAnalysis();
      }
    }

   StatusMessage(0, kTRUE, "Restart analysis loop");

   return kTRUE;
}

Bool_t TGo4Sniffer::CmdClose()
{
   auto an = TGo4Analysis::Instance();
   TGo4AnalysisClient *cli = an ? an->GetAnalysisClient() : nullptr;

   if (cli) {
      cli->Stop();
      an->CloseAnalysis();
    } else if (an) {
      an->StopAnalysis();
      an->PostLoop();
      an->CloseAnalysis();
    }

   StatusMessage(0, kTRUE, "Close analysis");

   return kTRUE;
}


Bool_t TGo4Sniffer::CmdExit()
{
   auto an = TGo4Analysis::Instance();
   TGo4AnalysisClient *cli = an ? an->GetAnalysisClient() : nullptr;

   if (cli) {
      cli->Stop();
      an->CloseAnalysis();
      cli->Quit();
   } else if (an) {
      an->StopWorking();
   }

   StatusMessage(0, kTRUE, "Exit analysis process");

   return kTRUE;
}


Bool_t TGo4Sniffer::CmdClearObject(const char *objname)
{
   auto ana = TGo4Analysis::Instance();

   if(!ana) {
      SendStatusMessage(3, kTRUE,"CmdClearObject - analysis missing");
      return kFALSE;
   }

   if (!objname || (*objname == 0)) {
      ana->ClearObjects("Histograms");
      ana->ClearObjects("Conditions");
      SendStatusMessage(1, kTRUE, "Histograms and conditions were cleared");
      return kTRUE;
   }

   Bool_t ok = ana->ClearObjects(objname);

   if(ok) {
      SendStatusMessage(1, kTRUE, TString::Format("Object %s was cleared.", objname));
   } else {
      SendStatusMessage(2, kTRUE, TString::Format("Could not clear object %s", objname));
   } // if(ob)

   return ok;
}

Bool_t TGo4Sniffer::CmdDeleteObject(const char *objname)
{
   auto ana = TGo4Analysis::Instance();

   if(!ana) {
      SendStatusMessage(3, kTRUE,"CmdDeleteObject - missing analysis ");
      return kFALSE;
   }

   if (!objname || (*objname == 0)) {
      SendStatusMessage(1, kTRUE, "Object name in CmdDeleteObject not specified");
      return kFALSE;
   }

   Bool_t ok = ana->DeleteObjects(objname);

   if(ok) {
      SendStatusMessage(1, kTRUE, TString::Format("Object %s was deleted", objname));
   } else {
      SendStatusMessage(2, kTRUE, TString::Format("Could not delete object %s", objname));
   } // if(ob)

   return ok;
}


Bool_t TGo4Sniffer::CmdExecute(const char *exeline)
{
   if (!exeline || (*exeline == 0)) return kFALSE;
   auto ana = TGo4Analysis::Instance();
   if (!ana) {
      SendStatusMessage(3, kTRUE, "CmdExecute - missing analysis ");
      return kFALSE;
   }
   Int_t errcode = 0;
   ana->ExecuteLine(exeline, &errcode);
   fflush(stdout);
   return errcode != 0 ? kFALSE : kTRUE;
}



void TGo4Sniffer::SetTitle(const char *title)
{
   // Method called by logger with every string, going to output

   if (!title || (strlen(title) == 0)) return;

   const char *prev = GetItemField("/Status/DebugOutput", "value");
   TString res;
   if (prev && (strcmp(prev,"---") != 0)) res = prev;
   if (res.Length() > 50000) res.Remove(0, res.Length() - 25000);
   res.Append("\n"); res.Append(title);

   SetItemField("/Status/DebugOutput","value", res);

   const char *cur = title;
   while (*cur != 0) {
      const char *next = strchr(cur, '\n');
      if (!next) {
         fDebugOutput.AddMsg(cur);
         break;
      }
      fDebugOutput.AddMsg(TString(cur, next-cur));
      cur = next+1;
   }
}

void TGo4Sniffer::RatemeterUpdate(TGo4Ratemeter* r)
{
   fRatemeter->UpdateFrom(r);

   Int_t n = fEventRate->GetN();
   if (n == 100) {
      fEventRate->RemovePoint(0);
      n--;
   }

   TTimeStamp tm, tm0;
   tm0.Set(1995,1,1,0,0,0,0,kTRUE,0);
   fEventRate->SetPoint(n, tm.AsDouble() - tm0.AsDouble(), r->GetRate());
   fEventRate->GetXaxis()->SetTimeDisplay(1);
   fEventRate->GetXaxis()->SetTimeFormat("%H:%M:%S");
   fEventRate->GetYaxis()->SetTitle("Events/s");
   fEventRate->GetHistogram()->SetDirectory(nullptr);
}

void TGo4Sniffer::StatusMessage(int level, Bool_t, const TString &msg)
{
   const char *prev = GetItemField("/Status/Message", "value");
   TString res;
   if (prev && (strcmp(prev,"---") != 0)) res = prev;
   res.Append("\n"); res.Append(msg);

   SetItemField("/Status/Message","value",res);

   TDatime now;

   fStatusMessages.AddMsg(TString::UItoa(now.Convert(kFALSE), 10) + ":" + TString::Itoa(level,10) + ":" + msg);

   // add status message to the log
   // if (printout) SetTitle(msg.Data());
}

void TGo4Sniffer::ProcessSnifferEvents()
{
   // Method called from the thread, where go4 analysis executed

   if (gHttpServer) gHttpServer->ProcessRequests();
}

void TGo4Sniffer::SendStatusMessage(Int_t level, Bool_t printout, const TString &text)
{
   if (printout)
      TGo4Log::Message(level, "%s", text.Data());

   // to be done
}

Bool_t TGo4Sniffer::AddAnalysisObject(TObject *obj)
{
   auto ana = TGo4Analysis::Instance();
   if (!ana) {
      SendStatusMessage(3, kFALSE, "Analysis not exists to set object");
      delete obj;
      return kFALSE;
   }

   Bool_t res = ana->AddHistogram(dynamic_cast<TH1 *>(obj));
   if (!res) res = ana->AddAnalysisCondition(dynamic_cast<TGo4Condition *>(obj));
   if (!res) res = ana->AddDynamicEntry(dynamic_cast<TGo4DynamicEntry *>(obj));

   if(res) {
      SendStatusMessage(1, kFALSE, TString::Format("Added new object %s to Go4 folders.", obj->GetName()));
   } else {
      SendStatusMessage(3, kFALSE, TString::Format("ERROR on adding new object %s ", obj->GetName()));
      delete obj;
   }

   return kTRUE;
}

Bool_t TGo4Sniffer::RemoteTreeDraw(const char *histoname,
                                   const char *treename,
                                   const char *varexpr,
                                   const char *cutexpr)
{
   auto ana = TGo4Analysis::Instance();
   if (!ana) {
      SendStatusMessage(3, kTRUE, "No analysis in RemoteTreeDraw");
      return kFALSE;
   }

   if (!histoname || (*histoname == 0)) histoname = "hTreeDraw";
   Bool_t res = ana->AddTreeHistogram(histoname, treename, varexpr, cutexpr);
   if(res)
      SendStatusMessage(1,kTRUE, TString::Format("Added Dynamic histogram %s for tree %s.", histoname, treename));
   else
      SendStatusMessage(2,kTRUE, TString::Format("Could not add Dynamic histogram %s for tree %s.", histoname, treename));

   return res;
}

TObject *TGo4Sniffer::CreateItemStatus(const char *itemname)
{
   TObject *obj = FindTObjectInHierarchy(itemname);

   TH1 *h1 = dynamic_cast<TH1 *> (obj);

   // printf("CreateItemStatus %s h1 = %p\n", itemname, h1);

   if (h1) return new TGo4HistogramStatus(h1);

   return nullptr;
}

Bool_t TGo4Sniffer::HasRestrictMethod()
{
   return IsA()->GetMethodAllAny("Restrict") != nullptr;
}

Bool_t TGo4Sniffer::HasProduceMultiMethod()
{
   return IsA()->GetMethodAllAny("ProduceMulti") != nullptr;
}

void TGo4Sniffer::RestrictGo4(const char *path, const char *options)
{
   // wrapper for TRootSnifferFull::Restrict, called only when method exists

   if (!HasRestrictMethod()) return;

   TString call_args = TString::Format("\"%s\",\"%s\"", path, options);

   TMethodCall call(IsA(), "Restrict", call_args.Data());

   call.Execute(this);
}

Bool_t TGo4Sniffer::HasAutoLoadMethod()
{
   return IsA()->GetMethodAllAny("SetAutoLoad") != nullptr;
}

Bool_t TGo4Sniffer::SetAutoLoadGo4(const char *script)
{
   if (!HasAutoLoadMethod()) return kFALSE;

   TString call_args = TString::Format("\"%s\"", script);

   TMethodCall call(IsA(), "SetAutoLoad", call_args.Data());

   call.Execute(this);

   return kTRUE;
}
