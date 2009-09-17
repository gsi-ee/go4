#include "TGo4Script.h"

#include "Riostream.h"
#include <stdlib.h>
#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TStopwatch.h"

#include "TGo4QSettings.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4DirProxy.h"
#include "TGo4HServProxy.h"
#include "TGo4Iter.h"
#include "TGo4Log.h"

#include "TGo4AnalysisConfiguration.h"
#include "TGo4ConfigStep.h"
#include "TGo4AnalysisWindow.h"
#include "TGo4WorkSpace.h"
#include "TGo4Picture.h"
#include "TGo4ViewPanel.h"
#include "TGo4MainWindow.h"

#include "TGo4Slot.h"

const char* TGo4Script::fgFileExtension = ".hotstart";
Int_t TGo4Script::fgDellayMillisec = 20;


TGo4Script* TGo4Script::ScriptInstance()
{
   return dynamic_cast<TGo4Script*> (Instance());
}

const char* TGo4Script::FileExtension()
{
   return fgFileExtension;
}

Int_t TGo4Script::DelayMillisec()
{
   return fgDellayMillisec;
}


TGo4Script::TGo4Script(TGo4MainWindow* mainwin) :
   TGo4AbstractInterface(),
   fiWaitForGUIReaction(0),
   fiWaitCounter(0),
   fStrBuf(),
   fMainWin(mainwin),
   fErrorFlag(kFALSE)
{
   if (mainwin!=0)
      Initialize(mainwin->OM(), mainwin->Browser());
}

TGo4Script::~TGo4Script()
{
}


Bool_t TGo4Script::StartScriptExecution(const char* fname)
{
   if ((fname==0) || (strlen(fname)==0)) return kFALSE;

   FinishExecution();

   if (!LoadHotStart(fname)) return kFALSE;

   fiWaitForGUIReaction = 0;
   fiWaitCounter = getCounts(0.1);

   doOutput("Start hostart script execution");

   return kTRUE;
}

Int_t TGo4Script::getCounts(Double_t tm)
{
   return int(tm*1000./DelayMillisec());
}

void  TGo4Script::doOutput(const char* str)
{
   if (fMainWin!=0)
     fMainWin->StatusMessage(str);
}

Bool_t TGo4Script::ContinueExecution()
{
   if ((fMainWin==0) || !IsHotStart()) return kFALSE;

   switch (execGUICommands()) {
      case 0:
         return kFALSE;
      case 1:
         fiWaitForGUIReaction = 0;
         fiWaitCounter = 0;
         break;
      case 2:
         fiWaitCounter--;
         return kTRUE;
   }

   const char* nextcmd = NextHotStartCmd();
   if (nextcmd==0) return kFALSE;

   Int_t error = 0;
   gROOT->ProcessLine(nextcmd, &error);

   if (error!=0) {
      doOutput(Form("Error = %d. CMD: %s", error, nextcmd));
      return kFALSE;
   }

   return kTRUE;
}

void TGo4Script::FinishExecution()
{
   FreeHotStartCmds();
   fErrorFlag = kFALSE;
   fiWaitForGUIReaction = 0;
   fiWaitCounter = 0;
}

Bool_t TGo4Script::IsWaitSomething()
{
   return fiWaitCounter>0;
}

void TGo4Script::DoPostProcessing()
{
   // do postprocessing of command only if normal script is executed
   if (IsHotStart()) return;

   // if error was before, nothing todo
   if (fErrorFlag || (fMainWin==0)) return;

   TStopwatch watch;

   Int_t loopnumbers = 0;
   Double_t spenttime = 0;

   do {

      loopnumbers++;
      Bool_t first = kTRUE;

      do {
         watch.Start();
         if (first) first = kFALSE;
               else gSystem->Sleep(DelayMillisec());
         fMainWin->ProcessQtEvents();
         watch.Stop();
         spenttime += watch.RealTime();
      } while (spenttime < loopnumbers*0.001*DelayMillisec());

      switch (execGUICommands()) {
         case 0: // error
            fiWaitForGUIReaction = 0;
            fiWaitCounter = 0;
            fErrorFlag = kTRUE;
            return;
         case 1: // finish command
            fiWaitForGUIReaction = 0;
            fiWaitCounter = 0;
            return;
         case 2: // next turn
            fiWaitCounter--;
      }
   } while (loopnumbers < 1000000);
}

void TGo4Script::ProcessEvents(Int_t timeout)
{
   Wait(timeout <= 0 ? 0 : timeout*0.001);
}

Int_t TGo4Script::execGUICommands()
{
   switch (fiWaitForGUIReaction) {
      case 0:
        return 1;

      case 1: { // execute lanch client method
         TGo4AnalysisProxy* anal = Analysis();

         if((anal!=0) && anal->IsAnalysisReady()) {
             doOutput("// Disconnect old analysis first");
             fMainWin->DisconnectAnalysisSlot(false);
             fiWaitForGUIReaction = 3;
             fiWaitCounter = getCounts(10.);
             return 2;
         }

         fMainWin->LaunchClientSlot(false);

         doOutput("// Start analysis client");

         // wait 5 sec for analysis ready
         fiWaitForGUIReaction = 2;
         fiWaitCounter = getCounts(5.);

         return 2;
      }

      case 2: {  // check if analysis is ready, can continue if not ok
         TGo4AnalysisProxy* anal = Analysis();
         if ((anal!=0) && anal->IsAnalysisReady()) return 1;
         return (fiWaitCounter<2) ? 1 : 2;
      }

      case 3: {   // wait while analysis is disconnected
         TGo4AnalysisProxy* anal = Analysis();
         if((anal==0) || !anal->IsAnalysisReady()) {
            fiWaitForGUIReaction = 4;
            fiWaitCounter = getCounts(5.);
            return 2;
         }
         return (fiWaitCounter<2) ? 0 : 2;
      }

      case 4: { // wait time and lunch analysis again
        if (fiWaitCounter<5)
          fiWaitForGUIReaction = 1;
        return 2;
      }

      case 5: {  // check if analysis is ready, abort if not ok
         TGo4AnalysisProxy* anal = Analysis();
         if (anal==0) return 0;
         if (anal->IsAnalysisReady()) return 1;
         return (fiWaitCounter<2) ? 0 : 2;
      }

      case 10: { // wait until submitted settings are set
        TGo4AnalysisProxy* anal = Analysis();
        if ((anal!=0) && anal->IsAnalysisSettingsReady()) {
          fiWaitForGUIReaction = 11;
          fiWaitCounter = getCounts(20.);
          return 2;
        }
        return (fiWaitCounter<2) ? 0 : 2;
      }

      case 11: { // wait until remote browser refresh it's data
         TGo4AnalysisProxy* anal = Analysis();
         if ((anal!=0) && anal->NamesListReceived()) return 1;
         return (fiWaitCounter<2) ? 0 : 2;
      }

      case 12: { // wait until analysis will be disconnected
         TGo4AnalysisProxy* anal = Analysis();
         if (anal==0) return 1;
         return (fiWaitCounter<2) ? 0 : 2;
      }

      case 15: { // connect to running server
         fMainWin->ConnectServerSlot(false, fStrBuf.Data());

         // wait 5 sec for analysis ready,
         fiWaitForGUIReaction = 2;
         fiWaitCounter = getCounts(5.);

         return 2;
      }

      case 111: { // just wait time and take next command
         return (fiWaitCounter<2) ? 1 : 2;
      }
   } //switch

   return 2;
}

// ******************* Functions to be used in GUI script *************

void TGo4Script::Wait(double tm_sec)
{
   fiWaitForGUIReaction = 111;
   fiWaitCounter = getCounts(tm_sec);
   if (fiWaitCounter<0) fiWaitCounter = 1;

   DoPostProcessing();
}


void TGo4Script::Message(const char* msg)
{
   if (fMainWin!=0)
     fMainWin->StatusMessage(msg);
}


void TGo4Script::HotStart(const char* filename)
{
   fMainWin->HotStart(filename);
}

void TGo4Script::LaunchAnalysis()
{
   fiWaitForGUIReaction = 1;
   fiWaitCounter = getCounts(1.);

   DoPostProcessing();
}

void TGo4Script::LaunchAnalysis(const char* ClientName,
                                const char* ClientDir,
                                const char* ClientExec,
                                const char* ClientNode,
                                Int_t ShellMode,
                                Int_t TermMode,
                                Int_t ExeMode)
{
   go4sett->setClientName(ClientName);
   go4sett->setClientDir(ClientDir);
   go4sett->setClientExec(ClientExec);
   go4sett->setClientNode(ClientNode);
   go4sett->setClientShellMode(ShellMode);
   go4sett->setClientTermMode(TermMode);
   go4sett->setClientExeMode(ExeMode);
   LaunchAnalysis();
}

void TGo4Script::ConnectAnalysis(const char* ServerNode,
                                 Int_t ServerPort,
                                 Int_t ControllerMode,
                                 const char* password)
{
   go4sett->setClientNode(ServerNode);
   go4sett->setClientPort(ServerPort);
   go4sett->setClientControllerMode(ControllerMode);
   go4sett->setClientDefaultPass((password==0) || (*password==0));
   fiWaitForGUIReaction = 15;
   fStrBuf = password;
   fiWaitCounter = getCounts(1.);

   DoPostProcessing();
}

void TGo4Script::WaitAnalysis(Int_t delay_sec)
{
   fiWaitForGUIReaction = 5;
   fiWaitCounter = getCounts(delay_sec);

   DoPostProcessing();
}

void TGo4Script::DisconnectAnalysis()
{
    fMainWin->DisconnectAnalysisSlot(false);

    fiWaitForGUIReaction = 12;
    fiWaitCounter = getCounts(20.);

    DoPostProcessing();
}

void TGo4Script::ShutdownAnalysis()
{
   fMainWin->ShutdownAnalysisSlot(false);

   fiWaitForGUIReaction = 12;
   fiWaitCounter = getCounts(20.);

   DoPostProcessing();
}

void TGo4Script::SubmitAnalysisConfig()
{
   fMainWin->SubmitAnalysisSettings();

   fiWaitForGUIReaction = 10;
   fiWaitCounter = getCounts(20.);

   DoPostProcessing();
}

void TGo4Script::StartAnalysis()
{
   fMainWin->StartAnalysisSlot();
   Wait(1.);
}

void TGo4Script::StopAnalysis()
{
   fMainWin->StopAnalysisSlot();
   Wait(2.);
}

void TGo4Script::SetAnalysisTerminalMode(int mode)
{
   TGo4AnalysisWindow* gui = fMainWin->FindAnalysisWindow();
   if (gui==0) return;

   if (mode<0) gui->close(); else
   if (mode==0) gui->showMinimized(); else
                gui->showNormal();
}

void TGo4Script::SetAnalysisConfigMode(int mode)
{
   TGo4AnalysisConfiguration* gui = fMainWin->FindAnalysisConfiguration();
   if (gui==0) return;

   if (mode<0) gui->close(); else
   if (mode==0) gui->showMinimized(); else
                gui->showNormal();
}

void TGo4Script::AnalysisAutoSave(const char* filename,
                                       Int_t interval,
                                       Int_t compression,
                                       Bool_t enabled,
                                       Bool_t overwrite)
{
   TGo4AnalysisConfiguration* gui = fMainWin->FindAnalysisConfiguration();
   if(gui!=0)
     gui->SetAutoSaveConfig(filename, interval, compression, enabled, overwrite);
}

void TGo4Script::AnalysisConfigName(const char* filename)
{
   TGo4AnalysisConfiguration* gui = fMainWin->FindAnalysisConfiguration();
   if(gui!=0)
      gui->SetAnalysisConfigFile(filename);
}

TGo4ConfigStep* TGo4Script::GetStepGUI(const char* stepname)
{
   TGo4AnalysisConfiguration* gui = fMainWin->FindAnalysisConfiguration();

   return gui==0 ? 0 : gui->FindStepConfig(stepname);
}

void TGo4Script::ConfigStep(const char* stepname,
                            Bool_t enableprocess,
                            Bool_t enablesource,
                            Bool_t enablestore)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step)
      step->SetStepControl(enableprocess, enablesource, enablestore);
}

void TGo4Script::StepFileSource(const char* stepname,
                                const char* sourcename,
                                int timeout)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout);
      step->SetFileSource();
   }
}

void TGo4Script::StepMbsFileSource(const char* stepname,
                                   const char* sourcename,
                                   int timeout,
                                   const char* TagFile,
                                   int start,
                                   int stop,
                                   int interval)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout, start, stop, interval);
      step->SetMbsFileSource(TagFile);
   }
}

void TGo4Script::StepMbsStreamSource(const char* stepname,
                                     const char* sourcename,
                                     int timeout,
                                     int start,
                                     int stop,
                                     int interval)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout, start, stop, interval);
      step->SetMbsStreamSource();
   }
}

void TGo4Script::StepMbsTransportSource(const char* stepname,
                                        const char* sourcename,
                                        int timeout,
                                        int start,
                                        int stop,
                                        int interval)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout, start, stop, interval);
      step->SetMbsTransportSource();
   }
}

void TGo4Script::StepMbsEventServerSource(const char* stepname,
                                          const char* sourcename,
                                          int timeout,
                                          int start,
                                          int stop,
                                          int interval)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout, start, stop, interval);
      step->SetMbsEventServerSource();
   }
}

void TGo4Script::StepMbsRevServSource(const char* stepname,
                                      const char* sourcename,
                                      int timeout,
                                      int port,
                                      int start,
                                      int stop,
                                      int interval)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout, start, stop, interval);
      step->SetMbsRevServSource(port);
   }
}

void TGo4Script::StepRandomSource(const char* stepname,
                                  const char* sourcename,
                                  int timeout)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout);
      step->SetRandomSource();
   }
}

void TGo4Script::StepUserSource(const char* stepname,
                                const char* sourcename,
                                int timeout,
                                int port,
                                const char* expr)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step) {
      step->ResetSourceWidgets(sourcename, timeout);
      step->SetUserSource(port, expr);
   }
}

void TGo4Script::StepFileStore(const char* stepname,
                               const char* storename,
                               bool overwrite,
                               int bufsize,
                               int splitlevel,
                               int compression)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step)
      step->SetFileStore(storename, overwrite, bufsize, splitlevel, compression);
}

void TGo4Script::StepBackStore(const char* stepname,
                               const char* storename,
                               int bufsize,
                               int splitlevel)
{
   TGo4ConfigStep* step = GetStepGUI(stepname);
   if (step)
      step->SetBackStore(storename, bufsize, splitlevel);
}

ViewPanelHandle TGo4Script::StartViewPanel()
{
   return fMainWin->MakeNewPanel(1);
}

ViewPanelHandle TGo4Script::StartViewPanel(int x, int y, int width, int height, int mode, TGo4Picture* pic)
{
   TGo4ViewPanel* panel = fMainWin->MakeNewPanel(1);
   if (panel==0) return 0;

   if (mode==0) panel->showMinimized(); else
   if (mode==2) panel->showMaximized(); else {
      panel->showNormal();
      panel->parentWidget()->move(QPoint(x,y));
      panel->parentWidget()->resize(QSize(width, height));
   }

   if (pic!=0) {
      panel->ProcessPictureRedraw("", panel->GetCanvas(), pic);
      panel->ShootRepaintTimer();
   }

   return (ViewPanelHandle) panel;
}

TString TGo4Script::GetViewPanelName(ViewPanelHandle handle)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if (panel==0) return TString();
   return TString(panel->objectName().toAscii().constData());
}

ViewPanelHandle TGo4Script::FindViewPanel(const char* name)
{
   return (ViewPanelHandle) fMainWin->FindViewPanel(name);
}

Bool_t TGo4Script::SetViewPanelName(ViewPanelHandle handle, const char* newname)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if ((handle==0) || (newname==0) || (strlen(newname)==0)) return kFALSE;

   if (fMainWin->FindViewPanel(newname)) {
      Message(Form("Viewpanel with name %s already exists",newname));
      return kFALSE;
   }

   panel->SetPanelName(newname);

   panel->UpdatePanelCaption();

   return kTRUE;
}

ViewPanelHandle TGo4Script::GetActiveViewPanel()
{
   return (ViewPanelHandle) TGo4WorkSpace::Instance()->GetActivePanel();
}

void TGo4Script::RedrawPanel(ViewPanelHandle handle)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if (panel!=0) panel->RedrawPanel(panel->GetCanvas(), true);
}

void TGo4Script::DivideViewPanel(ViewPanelHandle handle, Int_t numX, Int_t numY)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if (panel!=0) panel->Divide(numX, numY);
}

TPad* TGo4Script::SelectPad(ViewPanelHandle handle, Int_t number)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if (panel!=0) {
      TPad* pad = panel->GetSubPad(0, number, false);
      if (pad==0) pad = panel->GetCanvas();
      panel->SetActivePad(pad);
      ProcessEvents();
      return pad;
   }
   return 0;
}

TGo4Picture* TGo4Script::GetPadOptions(ViewPanelHandle handle, Int_t padnumber)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if (panel==0) return 0;

   TPad* pad = panel->GetSubPad(0, padnumber, false);
   if (pad==0) pad = panel->GetCanvas();
   return panel->GetPadOptions(pad);
}

void TGo4Script::SetSuperimpose(ViewPanelHandle handle, Bool_t on)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;
   if (panel!=0) panel->SetPadSuperImpose(panel->GetActivePad(), on);
}

Bool_t TGo4Script::DrawItem(const char* itemname, ViewPanelHandle handle, const char* drawopt)
{
   TGo4ViewPanel* panel = (TGo4ViewPanel*) handle;

   panel = fMainWin->DisplayBrowserItem(itemname, panel, 0, true, -1, drawopt);

   if (panel!=0) ProcessEvents();

   return (panel!=0);
}

// ***************************** Generation *********************


void TGo4Script::ProduceScript(const char* filename, TGo4MainWindow* main)
{
   TGo4AnalysisConfiguration* confgui = main->FindAnalysisConfiguration();

   TGo4AnalysisWindow* termgui = main->FindAnalysisWindow();

   TGo4BrowserProxy* br = main->Browser();

   TGo4AnalysisProxy* anal = br->FindAnalysis();

   ofstream fs(filename);

   fs << "// Automatically generated startup script" << endl;
   fs << "// Do not change it!" << endl << endl;

   ProduceLoadLibs(fs);

   TObjArray prlist;
   br->MakeFilesList(&prlist);
   for(Int_t n=0;n<=prlist.GetLast();n++) {
      TGo4DirProxy* pr = (TGo4DirProxy*) prlist.At(n);
      fs << "go4->OpenFile(\"" << pr->GetFileName() << "\");" << endl;
   }

   prlist.Clear();
   br->MakeHServerList(&prlist);
   for(Int_t n=0;n<=prlist.GetLast();n++) {
      TGo4HServProxy* pr = (TGo4HServProxy*) prlist.At(n);
      fs << "go4->ConnectHServer(\""
         << pr->GetServerName() << "\", "
         << pr->GetPortNumber() << ", \""
         << pr->GetBaseName() << "\", \""
         << pr->GetUserPass() << "\", \""
         << pr->GetFilter() << "\");" << endl;
   }

   fs << endl;

   if ((anal!=0) && anal->IsAnalysisReady() && !anal->IsAnalysisServer()) {
// start analysis configuration
   fs << "go4->LaunchAnalysis(\"" << go4sett->getClientName().toStdString() << "\", \""
                                  << go4sett->getClientDir().toStdString() <<  "\", \""
                                  << go4sett->getClientExec().toStdString() << "\", \""
                                  << go4sett->getClientNode().toStdString() << "\", "
                                  << go4sett->getClientShellMode() << ", "
                                  << go4sett->getClientTermMode() <<  ", "
                                  << go4sett->getClientExeMode() << ");" << endl;
   fs << "go4->WaitAnalysis(300);" << endl << endl;

   if (confgui==0) return;
   fs << "// configuration of analysis" << endl;

   QString fname;
   int interval, compression;
   bool asenabled, asoverwrite;

   confgui->GetAutoSaveConfig(fname, interval, compression, asenabled, asoverwrite);
   fs << "go4->AnalysisAutoSave(\"" << fname.toStdString() << "\", "
                                    << interval << ", "
                                    << compression << ", "
                                    << (asenabled ? "kTRUE" : "kFALSE") << ", "
                                    << (asoverwrite ? "kTRUE" : "kFALSE") << ");" << endl;

   confgui->GetAnalysisConfigFile(fname);
   fs << "go4->AnalysisConfigName(\"" << fname.toStdString() << "\");" << endl << endl;

   for(int nstep=0;nstep<confgui->GetNumSteps();nstep++) {
      TGo4ConfigStep* stepconf = confgui->GetStepConfig(nstep);
      fs << "// step " << stepconf->GetStepName().toStdString() << endl;

      bool process, source, store;
      stepconf->GetStepControl(process, source, store);
      fs << "go4->ConfigStep(\"" << stepconf->GetStepName().toStdString() << "\", "
                                 << (process ? "kTRUE" : "kFALSE") << ", "
                                 << (source ? "kTRUE" : "kFALSE") << ", "
                                 << (store ? "kTRUE" : "kFALSE") << ");" << endl;

      QString srcname;
      int timeout, start, stop, interval;
      int nsrc = stepconf->GetSourceSetup(srcname, timeout, start, stop, interval);
      TString srcargs = "(\"";
      srcargs += stepconf->GetStepName().toStdString();
      srcargs += "\", \"";
      srcargs += srcname.toStdString();
      srcargs += "\", ";
      srcargs += timeout;

      TString inter_args;
      if ((start!=0) || (stop!=0) || (interval!=0)) {
         inter_args += ", ";
         inter_args += start;
         inter_args += ", ";
         inter_args += stop;
         inter_args += ", ";
         inter_args += interval;
      }

      switch(nsrc) {
         case 0:
           fs << "go4->StepFileSource" << srcargs;
           break;
         case 1: {
           QString TagFile;
           stepconf->GetMbsFileSource(TagFile);
           fs << "go4->StepMbsFileSource" << srcargs << ", \""
                                          << TagFile.toStdString() << "\""
                                          << inter_args;
           break;
         }
         case 2:
           fs << "go4->StepMbsStreamSource" << srcargs << inter_args;
           break;
         case 3:
           fs << "go4->StepMbsTransportSource" << srcargs << inter_args;
           break;
         case 4:
           fs << "go4->StepMbsEventServerSource" << srcargs << inter_args;
           break;
         case 5: {
           int port;
           stepconf->GetMbsRevServSource(port);
           fs << "go4->StepMbsRevServSource" << srcargs << ", " << port << inter_args;
           break;
         }
         case 6:
           fs << "go4->StepRandomSource" << srcargs;
           break;
         case 7: {
           int port;
           QString expr;
           stepconf->GetUserSource(port, expr);
           fs << "go4->StepUserSource" << srcargs << ", " << port << ", \""
                                       << expr.toStdString() << "\"";
           break;
         }

      } //  switch(nsrc)
      fs << ");" << endl;

      QString storename;
      int nstore = stepconf->GetStoreSetup(storename);
      switch(nstore) {
         case 0: {
            bool overwrite;
            int bufsize, splitlevel, compression;
            stepconf->GetFileStore(overwrite, bufsize, splitlevel, compression);
            fs << "go4->StepFileStore(\"" << stepconf->GetStepName().toStdString() << "\", \""
                                          << storename.toStdString() << "\", "
                                          << (overwrite ? "kTRUE" : "kFALSE") << ", "
                                          << bufsize << ", "
                                          << splitlevel << ", "
                                          << compression << ");" << endl;
            break;
         }

         case 1: {
            int bufsize, splitlevel;
            stepconf->GetBackStore(bufsize, splitlevel);
            fs << "go4->StepBackStore(\"" << stepconf->GetStepName().toStdString() << "\", \""
                                          << storename.toStdString() << "\", "
                                          << bufsize << ", "
                                          << splitlevel << ");" << endl;
            break;
         }
      } // switch

      fs << endl;
   }

   if ((anal!=0) && anal->IsAnalysisSettingsReady())
      fs << "go4->SubmitAnalysisConfig();" << endl << endl;

   int mode = 1;
   if (confgui) {
      if (confgui->isHidden()) mode = -1; else
      if (confgui->isMinimized()) mode = 0;
   }
   fs << "go4->SetAnalysisConfigMode(" << mode << ");" << endl;

   mode = 1;
   if (termgui) {
      if (termgui->isHidden()) mode = -1; else
      if (termgui->isMinimized()) mode = 0;
   }
   fs << "go4->SetAnalysisTerminalMode(" << mode << ");" << endl << endl;
// end analysis configuration
   } else
   if ((anal!=0) && anal->IsAnalysisReady() && anal->IsAnalysisServer()) {
      fs << "go4->ConnectAnalysis(\""
         << go4sett->getClientNode().toStdString() << "\", "
         << go4sett->getClientPort() << ", "
         << go4sett->getClientControllerMode() << ", ";
      if (go4sett->getClientDefaultPass())
        fs << "0);" << endl;
      else
        fs << "\"" << main->LastTypedPassword() << "\");" << endl;
      fs << "go4->WaitAnalysis(10);" << endl << endl;
   } else {
      fs << "go4->DisconnectAnalysis();" << endl;
   }

   if ((anal!=0) && anal->IsAnalysisRunning() && !anal->IsAnalysisServer())
      fs << "go4->StartAnalysis();" << endl;

   int npanel=0;

   QWidgetList windows = TGo4WorkSpace::Instance()->windowList();
   for (int i=0; i<windows.count(); ++i ) {
      TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (windows.at(i));
      if (panel==0) continue;

      npanel++;
      TString picname = "pic";
      picname+=npanel;
      TGo4Picture pic(picname.Data(),"temporary object to setup viewpanel");

      panel->MakePictureForPad(&pic, panel->GetCanvas(), true);

      fs << endl;

      pic.SavePrimitive(fs,"");

      QPoint pos = panel->parentWidget()->pos();
      panel->parentWidget()->mapToParent(pos);
      QSize size = panel->parentWidget()->size();
      int mode = 1;
      if (panel->isHidden()) mode = -1; else
      if (panel->isMinimized()) mode = 0; else
      if (panel->isMaximized()) mode = 2;

      fs << "go4->StartViewPanel("
         << pos.x() << ", "
         << pos.y() << ", "
         << size.width() << ", "
         << size.height() << ", "
         << mode << ", " << picname << ");" << endl;

      fs << "delete " << picname << ";" << endl;
   }

   fs << endl;



   TGo4Iter iter(br->BrowserTopSlot(), kFALSE);
   Bool_t goinside = kTRUE;

   while (iter.next(goinside)) {
      TGo4Slot* subslot = iter.getslot();
      goinside = kTRUE;

      if (br->ItemKind(subslot)==TGo4Access::kndFolder) {
         // check if any subitem not monitored
         Int_t ncheck(0), ncount(0);
         TGo4Iter subiter(subslot, kTRUE);
         while (subiter.next()) {
            TGo4Slot* subsubslot = subiter.getslot();
            if (br->ItemKind(subsubslot)!=TGo4Access::kndObject) continue;
            ncount++;
            if (br->IsItemMonitored(subsubslot)) ncheck++;
                                         else break;
         }
         goinside = (ncount==0) || (ncheck!=ncount);
      } else
         goinside = !br->IsItemMonitored(subslot);

      if (!goinside) {
         TString sbuf;
         if (br->BrowserItemName(subslot, sbuf))
           fs << "go4->MonitorItem(\"" << sbuf << "\");" << endl;
      }

//      cout << "Test subslot: " << subslot->GetFullName() <<
//             " monitor= " << (goinside ? "false" : "true") << endl;

   }

   Int_t mperiod = br->MonitoringPeriod();
   if (mperiod>0)
     fs << "go4->StartMonitoring(" << mperiod/1000 << ");" << endl;
}
