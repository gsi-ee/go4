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

#ifndef TGO4SCRIPT_H
#define TGO4SCRIPT_H

#include "TGo4AbstractInterface.h"
#include "TString.h"

class TGo4MainWindow;
class TGo4ConfigStep;
class TGo4Picture;

class TGo4Script : public TGo4AbstractInterface {

   friend class TGo4MainWindow;

   protected:
       void ProcessEvents(Int_t timeout = -1) override;

   public:
      TGo4Script(TGo4MainWindow* mainwin);
      virtual ~TGo4Script();

      // general purpose
      void Wait(double tm_sec) override;

      void HotStart(const char *filename) override;

      void Message(const char *msg) override;

      // analysis start and configuration

      void LaunchAnalysis() override;
      void LaunchAnalysis(const char *ClientName,
                          const char *ClientDir,
                          const char *ClientExec,
                          const char *ClientNode,
                          Int_t ShellMode =  2,      // 0 = exec, 1 = rsh  2 = ssh
                          Int_t TermMode = 1,        // 1 = Qt   2 = Xterm 3 = konsole
                          Int_t ExeMode = 0,         // 0 = exe, 1 = lib
                          const char *UserArgs = nullptr) override;
      void ConnectAnalysis(const char *ServerNode,
                           Int_t ServerPort,
                           Int_t ControllerMode,  // 0 - obsrver, 1- controller, 2 - administrator
                           const char *password = nullptr)  override;

      void WaitAnalysis(Int_t delay_sec) override;
      void DisconnectAnalysis() override;
      void ShutdownAnalysis() override;

      void SubmitAnalysisConfig(int tmout = 20) override;
      void StartAnalysis() override;
      void StopAnalysis() override;
      void RefreshNamesList(int tmout = 10) override;


      void SetAnalysisTerminalMode(int mode) override;  // -1 - close, 0-minimized, 1-normal
      void SetAnalysisConfigMode(int mode) override;   // -1 - close, 0-minimized, 1-normal

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
                             const char *TagFile,
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
                                int port = 0,
                                int start = 0,
                                int stop = 0,
                                int interval = 0) override;

      void StepMbsSelection(const char *stepname,
                            int start,
                            int stop,
                            int interval) override;

      void StepRandomSource(const char *stepname,
                            const char *sourcename,
                            int timeout) override;

      void StepUserSource(const char *stepname,
                          const char *sourcename,
                          int timeout,
                          int port,
                          const char *expr) override;

      void StepHDF5Source(const char *stepname,
                          const char *sourcename,
                          int timeout) override;


      void StepMbsPort(const char *stepname,
                       int port) override;

      void StepMbsRetryCnt(const char *stepname,
                           int cnt) override;

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

      void StepHDF5Store(const char *stepname,
                         const char *storename,
                         int flags) override;

      // windows management

      void SetMainWindowState(int qtversion, const char *val) override;
      void SetMainWindowGeometry(int qtversion, const char *val) override;

      ViewPanelHandle StartViewPanel() override;
      ViewPanelHandle StartViewPanel(int x, int y, int width, int height, int mode = 1, TGo4Picture *pic = nullptr) override;
      TString GetViewPanelName(ViewPanelHandle panel) override;
      ViewPanelHandle FindViewPanel(const char *name) override;
      Bool_t SetViewPanelName(ViewPanelHandle panel, const char *newname) override;
      ViewPanelHandle GetActiveViewPanel() override;
      void RedrawPanel(ViewPanelHandle handle) override;
      void DivideViewPanel(ViewPanelHandle panel, Int_t numX, Int_t numY) override;
      TPad* SelectPad(ViewPanelHandle panel, Int_t number = 0) override;
      TGo4Picture *GetPadOptions(ViewPanelHandle panel, Int_t padnumber = 0) override;
      void SetSuperimpose(ViewPanelHandle panel, Bool_t on = kTRUE) override;
      void SetApplyToAll(ViewPanelHandle panel, Bool_t on = kTRUE) override;

      void StartFitPanel() override;

      Bool_t DrawItem(const char *itemname, ViewPanelHandle panel = nullptr, const char *drawopt = nullptr) override;

      const char *GetDrawnItemName(ViewPanelHandle panel, int cnt = 0) override;

      TGo4ServerProxy* ConnectHttp(const char *servername, const char *account = nullptr, const char *pass = nullptr) override;

   protected:

      static TGo4Script* ScriptInstance();

      Bool_t CanConfigureAnalysis();
      Bool_t StartScriptExecution(const char *fname);
      Bool_t ContinueExecution();
      void FinishExecution();

      Bool_t IsWaitSomething() const;

      TGo4ConfigStep* GetStepGUI(const char *stepname);

      static void ProduceScript(const char *filename, TGo4MainWindow* main);

      Int_t  getCounts(Double_t time);
      void   doOutput(const char *str);

      Int_t  execGUICommands();

      void  DoPostProcessing();

      Int_t     fiWaitForGUIReaction{0};    //!
      Int_t     fiWaitCounter{0};           //!
      TString   fStrBuf;                    //!
      TGo4MainWindow *fMainWin{nullptr};    //!
      Bool_t    fErrorFlag{kFALSE};         //!
      Int_t     fBlockConfigFlag{0};        //! block configuration until first start/stop command
};

#endif
