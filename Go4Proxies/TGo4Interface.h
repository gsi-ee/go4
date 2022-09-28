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

#ifndef TGO4INTERFACE_H
#define TGO4INTERFACE_H

#include "TGo4AbstractInterface.h"
#include "TString.h"

class TGo4RootBrowserProxy;
class TGo4AnalysisStatus;
class TGo4AnalysisStepStatus;
class TTimer;

class TGo4Interface : public TGo4AbstractInterface {
   protected:
       void ProcessEvents(Int_t timeout = -1) override;

   public:
      TGo4Interface();
      TGo4Interface(const TGo4Interface&) {}
      virtual ~TGo4Interface();

      void Wait(double tm_sec) override;
      void HotStart(const char *filename) override;
      void Message(const char* msg) override;

      // analysis start and configuration

      void LaunchAnalysis() override;
      void LaunchAnalysis(const char* ClientName,
                          const char* ClientDir,
                          const char* ClientExec,
                          const char* ClientNode,
                          Int_t ShellMode =  2,     // 0 = exec, 1 = rsh  2 = ssh
                          Int_t TermMode = 1,       // 1 = Qt   2 = Xterm 3 = konsole
                          Int_t ExeMode = 0,        // 0 = exe, 1 = lib
                          const char* UserArgs = nullptr) override;
      void ConnectAnalysis(const char* ServerNode,
                           Int_t ServerPort,
                           Int_t ControllerMode,  // 0 - obsrver, 1- controller, 2 - administrator
                           const char* password = nullptr) override;
      void WaitAnalysis(Int_t delay_sec) override;
      void DisconnectAnalysis() override;
      void ShutdownAnalysis() override;

      void SubmitAnalysisConfig(int tmout = 20) override;
      void StartAnalysis() override;
      void StopAnalysis() override;
      void RefreshNamesList(int tmout = 10) override;


      // steps configuration

      void AnalysisAutoSave(const char *filename,
                            Int_t interval,
                            Int_t compression,
                            Bool_t enabled,
                            Bool_t overwrite) override;

      void AnalysisConfigName(const char *filename) override;

      void ConfigStep(const char *stepname,
                      Bool_t enableprocess,
                      Bool_t enablesource,
                      Bool_t enablestore) override;

      void StepFileSource(const char *stepname,
                          const char *sourcename,
                          int timeout) override;

      void StepMbsFileSource(const char *stepname,
                             const char *sourcename,
                             int timeout,
                             const char* TagFile,
                             int start = 0,
                             int stop = 0,
                             int interval = 0) override;

      void StepMbsStreamSource(const char *stepname,
                               const char *sourcename,
                               int timeout,
                               int start = 0,
                               int stop = 0,
                               int interval = 0) override;

      void StepMbsTransportSource(const char *stepname,
                                  const char *sourcename,
                                  int timeout,
                                  int start = 0,
                                  int stop = 0,
                                  int interval = 0) override;

      void StepMbsEventServerSource(const char *stepname,
                                    const char *sourcename,
                                    int timeout,
                                    int start = 0,
                                    int stop = 0,
                                    int interval = 0) override;

      void StepMbsRevServSource(const char *stepname,
                                const char *sourcename,
                                int timeout,
                                int port,
                                int start = 0,
                                int stop = 0,
                                int interval = 0) override;

      void StepRandomSource(const char *stepname,
                            const char *sourcename,
                            int timeout) override;

      void StepUserSource(const char *stepname,
                          const char *sourcename,
                          int timeout,
                          int port,
                          const char *expr) override;

      void StepFileStore(const char *stepname,
                         const char *storename,
                         bool overwrite,
                         int bufsize,
                         int splitlevel,
                         int compression,
                         int autosaveperiod = 10000) override;

      void StepBackStore(const char *stepname,
                         const char *storename,
                         int bufsize,
                         int splitlevel) override;

      void StepUserStore(const char *stepname,
                         const char *storename) override;

     // windows management
      ViewPanelHandle StartViewPanel() override;
      ViewPanelHandle StartViewPanel(int x, int y, int width, int height, int mode = 1, TGo4Picture* pic = nullptr) override;
      TString GetViewPanelName(ViewPanelHandle handle) override;
      ViewPanelHandle FindViewPanel(const char *name) override;
      Bool_t SetViewPanelName(ViewPanelHandle handle, const char* newname) override;
      ViewPanelHandle GetActiveViewPanel() override;
      void RedrawPanel(ViewPanelHandle panel) override;
      void DivideViewPanel(ViewPanelHandle panel, Int_t numX, Int_t numY) override;
      TPad* SelectPad(ViewPanelHandle panel, Int_t number = 0) override;

      Bool_t DrawItem(const char *itemname, ViewPanelHandle panel = nullptr, const char* drawopt = nullptr) override;
      void RedrawItem(const char *itemname) override;

      void ProcessHotStart();

      Bool_t HandleTimer(TTimer* timer) override;

   protected:
      TGo4AnalysisProxy*  AddAnalysisProxy(Bool_t isserver);
      TGo4AnalysisStatus* GetAnalStatus();
      TGo4AnalysisStepStatus* GetStepStatus(const char *stepname);

      TGo4RootBrowserProxy* fRootBrowser{nullptr}; //!
      TString fOMDataPath;                         //!
      TString fOMBrowserPath;                      //!
      TString fOMEditorsPath;                      //!
      Bool_t fCmdFinished{kFALSE};                 //!
      TTimer* fCmdTimer{nullptr};                  //!
      Int_t   fWaitCounter{0};                     //!


   ClassDefOverride(TGo4Interface, 1);
};

#endif
