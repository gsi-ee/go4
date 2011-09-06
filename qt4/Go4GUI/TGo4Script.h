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
       void ProcessEvents(Int_t timeout = -1);

   public:
      TGo4Script(TGo4MainWindow* mainwin);
      virtual ~TGo4Script();

      // general purpose
      void Wait(double tm_sec);

      void HotStart(const char* filename);

      void Message(const char* msg);

      // analysis start and configuration

      void LaunchAnalysis();
      void LaunchAnalysis(const char* ClientName,
                          const char* ClientDir,
                          const char* ClientExec,
                          const char* ClientNode,
                          Int_t ShellMode =  2,      // 0 = exec, 1 = rsh  2 = ssh
                          Int_t TermMode = 1,        // 1 = Qt   2 = Xterm 3 = konsole
                          Int_t ExeMode = 0,         // 0 = exe, 1 = lib
                          const char* UserArgs = 0);
      void ConnectAnalysis(const char* ServerNode,
                           Int_t ServerPort,
                           Int_t ControllerMode,  // 0 - obsrver, 1- controller, 2 - administrator
                           const char* password = 0);

      void WaitAnalysis(Int_t delay_sec);
      void DisconnectAnalysis();
      void ShutdownAnalysis();

      void SubmitAnalysisConfig();
      void StartAnalysis();
      void StopAnalysis();

      void SetAnalysisTerminalMode(int mode);  // -1 - close , 0-minimized, 1-normal
      void SetAnalysisConfigMode(int mode);   // -1 - close , 0-minimized, 1-normal

      // steps configuration

      void AnalysisAutoSave(const char* filename,
                            Int_t interval,
                            Int_t compression,
                            Bool_t enabled,
                            Bool_t overwrite);

      void AnalysisConfigName(const char* filename);

      void ConfigStep(const char* stepname,
                      Bool_t enableprocess,
                      Bool_t enablesource,
                      Bool_t enablestore);

      void StepFileSource(const char* stepname,
                          const char* sourcename,
                          int timeout);

      void StepMbsFileSource(const char* stepname,
                             const char* sourcename,
                             int timeout,
                             const char* TagFile,
                             int start = 0,
                             int stop = 0,
                             int interval = 0);

      void StepMbsStreamSource(const char* stepname,
                               const char* sourcename,
                               int timeout,
                               int start = 0,
                               int stop = 0,
                               int interval = 0);

      void StepMbsTransportSource(const char* stepname,
                                  const char* sourcename,
                                  int timeout,
                                  int start = 0,
                                  int stop = 0,
                                  int interval = 0);

      void StepMbsEventServerSource(const char* stepname,
                                    const char* sourcename,
                                    int timeout,
                                    int start = 0,
                                    int stop = 0,
                                    int interval = 0);

      void StepMbsRevServSource(const char* stepname,
                                const char* sourcename,
                                int timeout,
                                int port = 0,
                                int start = 0,
                                int stop = 0,
                                int interval = 0);

      void StepRandomSource(const char* stepname,
                            const char* sourcename,
                            int timeout);

      void StepUserSource(const char* stepname,
                          const char* sourcename,
                          int timeout,
                          int port,
                          const char* expr);

      void StepMbsPort(const char* stepname,
                       int port);

      void StepFileStore(const char* stepname,
                         const char* storename,
                         bool overwrite,
                         int bufsize,
                         int splitlevel,
                         int compression);

      void StepBackStore(const char* stepname,
                         const char* storename,
                         int bufsize,
                         int splitlevel);

      // windows management

      ViewPanelHandle StartViewPanel();
      ViewPanelHandle StartViewPanel(int x, int y, int width, int height, int mode = 1, TGo4Picture* pic = 0);
      TString GetViewPanelName(ViewPanelHandle panel);
      ViewPanelHandle FindViewPanel(const char* name);
      Bool_t SetViewPanelName(ViewPanelHandle panel, const char* newname);
      ViewPanelHandle GetActiveViewPanel();
      void RedrawPanel(ViewPanelHandle handle);
      void DivideViewPanel(ViewPanelHandle panel, Int_t numX, Int_t numY);
      TPad* SelectPad(ViewPanelHandle panel, Int_t number = 0);
      TGo4Picture* GetPadOptions(ViewPanelHandle panel, Int_t padnumber = 0);
      void SetSuperimpose(ViewPanelHandle panel, Bool_t on = kTRUE);

      Bool_t DrawItem(const char* itemname, ViewPanelHandle panel = 0, const char* drawopt = 0);

   protected:

      static TGo4Script* ScriptInstance();

      Bool_t StartScriptExecution(const char* fname);
      Bool_t ContinueExecution();
      void FinishExecution();

      Bool_t IsWaitSomething();

      TGo4ConfigStep* GetStepGUI(const char* stepname);

      static void ProduceScript(const char* filename, TGo4MainWindow* main);

      Int_t  getCounts(Double_t time);
      void   doOutput(const char* str);

      Int_t  execGUICommands();

      void  DoPostProcessing();

      Int_t     fiWaitForGUIReaction;  //!
      Int_t     fiWaitCounter;         //!
      TString   fStrBuf;               //!
      TGo4MainWindow* fMainWin;        //!
      Bool_t    fErrorFlag;            //!

   ClassDef(TGo4Script, 1);
};

#endif
