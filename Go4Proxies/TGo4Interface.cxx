#include "TGo4Interface.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TTimer.h"

#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisStepStatus.h"

#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"
#include "TGo4UserSourceParameter.h"
#include "TGo4RevServParameter.h"
#include "TGo4FileStoreParameter.h"
#include "TGo4BackStoreParameter.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4MbsFileParameter.h"
#include "TGo4MbsStreamParameter.h"
#include "TGo4MbsTransportParameter.h"
#include "TGo4MbsEventServerParameter.h"
#include "TGo4MbsRandomParameter.h"

#include "TGo4ObjectManager.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4RootBrowserProxy.h"

TGo4Interface::TGo4Interface() :
   TGo4AbstractInterface(),
   fRootBrowser(0),
   fOMDataPath(),
   fOMBrowserPath(),
   fOMEditorsPath(),
   fCmdFinished(kTRUE),
   fCmdTimer(0),
   fWaitCounter(0)
{
   // create object manager
   TGo4ObjectManager* om =
      new TGo4ObjectManager("GUI_OM","Gui object manager");
   fOMDataPath    = "data";
   fOMBrowserPath = "gui";
   fOMEditorsPath = "editors";

   // create mount point for all data sources
   om->MakeFolder(fOMDataPath.Data());

   // create entry for browser
   TGo4BrowserProxy* br =
      new TGo4BrowserProxy(fOMDataPath.Data(), fOMEditorsPath.Data(), kTRUE);
   om->AddProxy("", br, fOMBrowserPath.Data(), "Place for gui slots");
   br->CreateMemoryFolder();

   // create entry for gui browser
   fRootBrowser = new TGo4RootBrowserProxy(br);
   om->AddProxy("", fRootBrowser, fOMEditorsPath.Data(), "Proxy for ROOT browser");
   om->AddLink(br->BrowserTopSlot(), fOMEditorsPath.Data(), "Go4Browser", "Link to Go4 browser");

   Initialize(om, br);
}

TGo4Interface::~TGo4Interface()
{
   DisconnectAnalysis();
   delete fCmdTimer;
   fCmdTimer = 0;

   Int_t timecnt = 50;
   while (timecnt-->0) {
      if (Analysis()==0) break;
      gSystem->ProcessEvents();
      gSystem->Sleep(100);
   }

   TGo4ObjectManager* om = OM();
   delete om;
}

void TGo4Interface::ProcessEvents(Int_t timeout)
{
   Wait(timeout<=0 ? 0 : timeout*0.001);
}

void TGo4Interface::Wait(double tm_sec)
{
   TStopwatch watch;

   Double_t spenttime = 0.;
   Bool_t first = kTRUE;

   do {
      watch.Start();
      if (first) first = kFALSE;
            else gSystem->Sleep(50);
      gSystem->ProcessEvents();
      watch.Stop();
      spenttime += watch.RealTime();
   } while (spenttime <= tm_sec);
}

void TGo4Interface::Message(const char* msg)
{
    if (fRootBrowser)
       fRootBrowser->Message("Message","msg",5000);
}

void TGo4Interface::LaunchAnalysis()
{
   // do nothing while no parameters is specified
   // work in Qt GUI while parameters can be taken from the QSettings
}

TGo4AnalysisProxy* TGo4Interface::AddAnalysisProxy(Bool_t isserver)
{
   const char* analisysitem = "Analysis";

   TGo4Slot* analslot = Browser()->DataSlot(analisysitem);

   if (analslot==0) {
      TGo4AnalysisProxy* anal = new TGo4AnalysisProxy(isserver);
      OM()->AddProxy(fOMDataPath.Data(), anal, analisysitem, "Analysis proxy");
      analslot = Browser()->DataSlot(analisysitem);
   }

   TGo4AnalysisProxy* anal = (analslot==0) ? 0 :
      dynamic_cast<TGo4AnalysisProxy*> (analslot->GetProxy());

   if (anal!=0)
     anal->SetAnalysisReady(kFALSE);

   if ((anal!=0) && (fRootBrowser!=0)) {
      OM()->AddLink(anal->RatemeterSlot(), fOMEditorsPath.Data(), "AnalRateMeter", "Analysis ratemeter");
      OM()->AddLink(anal->LoginfoSlot(), fOMEditorsPath.Data(), "AnalLoginfo", "Analysis loginfo");
   }

   return anal;
}


void TGo4Interface::LaunchAnalysis(const char* ClientName,
                                   const char* ClientDir,
                                   const char* ClientExec,
                                   const char* ClientNode,
                                   Int_t ShellMode,
                                   Int_t TermMode,
                                   Int_t ExeMode)
{
   TString launchcmd, killcmd;

   TGo4AnalysisProxy* anal = AddAnalysisProxy(kFALSE);
   if (anal==0) return;

   anal->LaunchAsClient(launchcmd, killcmd,
                       (ShellMode==2),
                       TermMode==2 ? 2 : 3,
                       ClientName,
                       ClientNode,
                       ClientDir,
                       ClientExec,
                       ExeMode);

   gSystem->ProcessEvents();

   ProcessEvents(200);
}

void TGo4Interface::ConnectAnalysis(const char* ServerNode,
                                    Int_t ServerPort,
                                    Int_t ControllerMode,
                                    const char* password)
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) anal = AddAnalysisProxy(kTRUE);

   if (anal==0) return;

   anal->ConnectToServer(ServerNode,
                         ServerPort,
                         ControllerMode,
                         password);

   if (ControllerMode>0)
     anal->RequestAnalysisSettings();

//   gSystem->ProcessEvents();
   ProcessEvents(200);
}

void TGo4Interface::WaitAnalysis(Int_t delay_sec)
{
   if (IsHotStart()) {
      fWaitCounter = delay_sec*100;
      return;
   }

   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) {
       Error("WaitAnalysis","Analysis not found");
       return;
   }

   delay_sec*=10;

   while (delay_sec-->0) {
      gSystem->ProcessEvents();
      gSystem->Sleep(100);
      gSystem->ProcessEvents();
      if (anal->IsAnalysisReady())
         return;
   }
}

void TGo4Interface::DisconnectAnalysis()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) return;

   Browser()->ToggleMonitoring(0);

   anal->DisconnectAnalysis(5, kFALSE);

   ProcessEvents(200);
}

void TGo4Interface::ShutdownAnalysis()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) return;

   Browser()->ToggleMonitoring(0);

   bool realshutdown = anal->IsAnalysisServer() &&
                       anal->IsConnected() &&
                       anal->IsAdministrator();

   anal->DisconnectAnalysis(30, realshutdown);
}

void TGo4Interface::SubmitAnalysisConfig()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) return;

   anal->SubmitAnalysisSettings();
   anal->RefreshNamesList();

   Int_t delay_sec = 20;
   while (delay_sec-->0) {
      gSystem->ProcessEvents();
      gSystem->Sleep(1000);
      gSystem->ProcessEvents();
      if (anal->IsAnalysisSettingsReady())
        if (anal->NamesListReceived()) return;
   }
}

void TGo4Interface::StartAnalysis()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) return;

   anal->StartAnalysis();
   anal->RefreshNamesList();
   anal->DelayedRefreshNamesList(4);

   gSystem->ProcessEvents();
   gSystem->Sleep(500);
   gSystem->ProcessEvents();
}

void TGo4Interface::StopAnalysis()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) return;

   anal->StopAnalysis();

   gSystem->ProcessEvents();
   gSystem->Sleep(1000);
   gSystem->ProcessEvents();
}

TGo4AnalysisStatus* TGo4Interface::GetAnalStatus()
{
   TGo4AnalysisProxy* anal = Analysis();
   if (anal==0) return 0;
   if (anal->SettingsSlot()==0) return 0;

   return dynamic_cast<TGo4AnalysisStatus*>
            (anal->SettingsSlot()->GetAssignedObject());
}

void TGo4Interface::AnalysisAutoSave(const char* filename,
                                         Int_t interval,
                                         Int_t compression,
                                         Bool_t enabled,
                                         Bool_t overwrite)
{
   TGo4AnalysisStatus* status  = GetAnalStatus();
   if (status==0) return;

   status->SetAutoFileName(filename);
   status->SetAutoSaveInterval(interval);
   status->SetAutoSaveCompression(compression);
   status->SetAutoSaveOn(enabled);
   status->SetAutoSaveOverwrite(overwrite);
}

void TGo4Interface::AnalysisConfigName(const char* filename)
{
   TGo4AnalysisStatus* status  = GetAnalStatus();
   if (status!=0)
     status->SetConfigFileName(filename);
}

TGo4AnalysisStepStatus* TGo4Interface::GetStepStatus(const char* stepname)
{
   TGo4AnalysisStatus* status = GetAnalStatus();
   return status==0 ? 0 : status->GetStepStatus(stepname);
}

void TGo4Interface::ConfigStep(const char* stepname,
                                   Bool_t enableprocess,
                                   Bool_t enablesource,
                                   Bool_t enablestore)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   step->SetSourceEnabled(enablesource);
   step->SetStoreEnabled(enablestore);
   step->SetProcessEnabled(enableprocess);
}

void TGo4Interface::StepFileSource(const char* stepname,
                                       const char* sourcename,
                                       int timeout)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4FileSourceParameter par(sourcename);
   par.SetTimeout(timeout);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepMbsFileSource(const char* stepname,
                                      const char* sourcename,
                                      int timeout,
                                      const char* TagFile,
                                      int start,
                                      int stop,
                                      int interval)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4MbsFileParameter par(sourcename);
   par.SetTimeout(timeout);

   if(TagFile==0) par.SetTagName("GO4-NOLMDTAG");
             else par.SetTagName(TagFile);
   par.SetStartEvent(start);
   par.SetStopEvent(stop);
   par.SetEventInterval(interval);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepMbsStreamSource(const char* stepname,
                                        const char* sourcename,
                                        int timeout,
                                        int start,
                                        int stop,
                                        int interval)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4MbsStreamParameter par(sourcename);
   par.SetTimeout(timeout);
   par.SetStartEvent(start);
   par.SetStopEvent(stop);
   par.SetEventInterval(interval);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepMbsTransportSource(const char* stepname,
                                        const char* sourcename,
                                        int timeout,
                                        int start,
                                        int stop,
                                        int interval)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4MbsTransportParameter par(sourcename);
   par.SetTimeout(timeout);
   par.SetStartEvent(start);
   par.SetStopEvent(stop);
   par.SetEventInterval(interval);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepMbsEventServerSource(const char* stepname,
                                          const char* sourcename,
                                          int timeout,
                                          int start,
                                          int stop,
                                          int interval)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4MbsEventServerParameter par(sourcename);
   par.SetTimeout(timeout);
   par.SetStartEvent(start);
   par.SetStopEvent(stop);
   par.SetEventInterval(interval);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepMbsRevServSource(const char* stepname,
                                      const char* sourcename,
                                      int timeout,
                                      int port,
                                      int start,
                                      int stop,
                                      int interval)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4RevServParameter par(sourcename);
   par.SetTimeout(timeout);
   par.SetPort(port);
   par.SetStartEvent(start);
   par.SetStopEvent(stop);
   par.SetEventInterval(interval);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepRandomSource(const char* stepname,
                                  const char* sourcename,
                                  int timeout)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4MbsRandomParameter par(sourcename);
   par.SetTimeout(timeout);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepUserSource(const char* stepname,
                                const char* sourcename,
                                int timeout,
                                int port,
                                const char* expr)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4UserSourceParameter par(sourcename);
   par.SetTimeout(timeout);
   par.SetPort(port);
   par.SetExpression(expr);
   step->SetSourcePar(&par);
}

void TGo4Interface::StepFileStore(const char* stepname,
                               const char* storename,
                               bool overwrite,
                               int bufsize,
                               int splitlevel,
                               int compression)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4FileStoreParameter par(storename);
   par.SetOverwriteMode(overwrite);
   par.SetBufsize(bufsize);
   par.SetSplitlevel(splitlevel);
   par.SetCompression(compression);
   step->SetStorePar(&par);
}

void TGo4Interface::StepBackStore(const char* stepname,
                               const char* storename,
                               int bufsize,
                               int splitlevel)
{
   TGo4AnalysisStepStatus* step = GetStepStatus(stepname);
   if (step==0) return;

   TGo4BackStoreParameter par(storename);
   par.SetBufsize(bufsize);
   par.SetSplitlevel(splitlevel);
   step->SetStorePar(&par);
}

ViewPanelHandle TGo4Interface::StartViewPanel()
{
   return StartViewPanel(10,10, 500, 300, 1, 0);
}

ViewPanelHandle TGo4Interface::StartViewPanel(int x, int y, int width, int height, int mode, TGo4Picture* pic)
{
   static Int_t cancounter = 0;

   TString cname = "Panel";
   cname+=cancounter++;

   TCanvas* c = new TCanvas(cname.Data(), TString("Drawing of ") + cname, width, height);

   fRootBrowser->DrawPicture("", pic, c);

   c->Update();

   return (ViewPanelHandle) c;
}

TString TGo4Interface::GetViewPanelName(ViewPanelHandle handle)
{
   TCanvas* c = (TCanvas*) handle;
   return TString(c ? c->GetName() : "");
}

ViewPanelHandle TGo4Interface::FindViewPanel(const char* name)
{
   return (ViewPanelHandle) gROOT->GetListOfCanvases()->FindObject(name);
}

Bool_t TGo4Interface::SetViewPanelName(ViewPanelHandle handle, const char* newname)
{
   TCanvas* c = (TCanvas*) handle;
   if ((c==0) || (newname==0) || (strlen(newname)==0)) return kFALSE;

   if (gROOT->GetListOfCanvases()->FindObject(newname)) {
      Message(Form("Canvas with name %s already exists",newname));
      return kFALSE;
   }

   c->SetName(newname);

   c->Update();

   return kTRUE;
}

ViewPanelHandle TGo4Interface::GetActiveViewPanel()
{
   if (gPad==0) return 0;
   return (ViewPanelHandle) gPad->GetCanvas();
}

void TGo4Interface::RedrawPanel(ViewPanelHandle handle)
{
   TCanvas* c = (TCanvas*) handle;
   if (c!=0) {
      c->Modified();
      c->Update();
   }
}

void TGo4Interface::DivideViewPanel(ViewPanelHandle handle, Int_t numX, Int_t numY)
{
    TCanvas* c = (TCanvas*) handle;
    if (c!=0) c->Divide(numX, numY);
}

TPad* TGo4Interface::SelectPad(ViewPanelHandle handle, Int_t number)
{
    TCanvas* c = (TCanvas*) handle;
    if (c!=0) return (TPad*) c->cd(number);
    return 0;
}

Bool_t TGo4Interface::DrawItem(const char* itemname, ViewPanelHandle handle, const char* drawopt)
{
    if (handle==0) handle = StartViewPanel();

    TObject* obj = GetObject(itemname, 5000);

    if (obj!=0) obj->Draw(drawopt);

    return obj!=0;
}

void TGo4Interface::RedrawItem(const char* itemname)
{
   TGo4AbstractInterface::RedrawItem(itemname);

   TIter next(gROOT->GetListOfCanvases());
   TPad* pad = 0;
   while ((pad = (TPad*) next()) != 0) {
      pad->Modified();

      TVirtualPad* subpad = 0;
      Int_t number = 0;
      while ((subpad = pad->GetPad(number++))!=0)
        subpad->Modified();

      pad->Update();
   }
}

Bool_t TGo4Interface::HandleTimer(TTimer* timer)
{
   // if there is no commands remains, just exit
   if (!IsHotStart()) return kTRUE;

   if (timer!=fCmdTimer) return kFALSE;

   // for the moment, only WaitAnalysis requries that strange waiting
   // during hotstart. In normal script simple ProcessEvents and Sleep works

   if (fWaitCounter>0) {
       fWaitCounter--;
       TGo4AnalysisProxy* anal = Analysis();
       if (anal && anal->IsAnalysisReady())
          fWaitCounter = 0;
       else
          if (fWaitCounter<2) FreeHotStartCmds(); // if cannot wait analysis, just break execution
   } else
      ProcessHotStart();

   if (IsHotStart()) fCmdTimer->Start(10, kTRUE);
   return kTRUE;
}


void TGo4Interface::HotStart(const char* filename)
{
   if ((filename==0) || (strlen(filename)==0)) return;

   FreeHotStartCmds();

   fCmdFinished = kTRUE;

   if (!LoadHotStart(filename)) return;

   if (fCmdTimer==0)
      fCmdTimer = new TTimer(this, 10, kTRUE);

   fCmdTimer->Start(10, kTRUE);
}

void TGo4Interface::ProcessHotStart()
{
   if (!fCmdFinished) {
       Error("ProcessHotStart","Internal problem");
       return;
   }

   const char* nextcmd = NextHotStartCmd();

   if ((nextcmd==0) || !IsHotStart()) return;

   Int_t error = 0;
   fCmdFinished = kFALSE;
   gROOT->ProcessLineSync(nextcmd, &error);
   fCmdFinished = kTRUE;

   if (error!=0)
      Error("ProcessHotStart", "ProcessLine(\"%s\") error %d", nextcmd, error);
}
