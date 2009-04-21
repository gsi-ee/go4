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
       void ProcessEvents(Int_t timeout = -1);

   public:
      TGo4Interface();
      TGo4Interface(const TGo4Interface&) {}
      virtual ~TGo4Interface();

      void Wait(double tm_sec);
      void HotStart(const char* filename);
      void Message(const char* msg);

      // analysis start and configuration

      void LaunchAnalysis();
      void LaunchAnalysis(const char* ClientName,
                          const char* ClientDir,
                          const char* ClientExec,
                          const char* ClientNode,
                          Int_t ShellMode =  2,      // 1 = rsh  2 = ssh
                          Int_t TermMode = 1);      // 1 = Qt   2 = Xterm 3 = konsole
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
                                int port,
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
      TString GetViewPanelName(ViewPanelHandle handle);
      ViewPanelHandle FindViewPanel(const char* name);
      Bool_t SetViewPanelName(ViewPanelHandle handle, const char* newname);
      ViewPanelHandle GetActiveViewPanel();
      void RedrawPanel(ViewPanelHandle panel);
      void DivideViewPanel(ViewPanelHandle panel, Int_t numX, Int_t numY);
      TPad* SelectPad(ViewPanelHandle panel, Int_t number = 0);

      Bool_t DrawItem(const char* itemname, ViewPanelHandle panel = 0, const char* drawopt = 0);
      void RedrawItem(const char* itemname);

      void ProcessHotStart();

      virtual Bool_t HandleTimer(TTimer* timer);

   protected:
      TGo4AnalysisProxy*  AddAnalysisProxy(Bool_t isserver);
      TGo4AnalysisStatus* GetAnalStatus();
      TGo4AnalysisStepStatus* GetStepStatus(const char* stepname);

      TGo4RootBrowserProxy* fRootBrowser; //!
      TString fOMDataPath;           //!
      TString fOMBrowserPath;        //!
      TString fOMEditorsPath;        //!
      Bool_t fCmdFinished;            //!
      TTimer* fCmdTimer;              //!
      Int_t   fWaitCounter;          //!


   ClassDef(TGo4Interface, 1);

};
#endif
