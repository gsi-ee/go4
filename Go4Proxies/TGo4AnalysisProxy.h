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

#ifndef TGO4ANALYSISPROXY_H
#define TGO4ANALYSISPROXY_H

#include "TGo4ServerProxy.h"

#include "TObjArray.h"

class TGo4AnalysisObjectNames;
class TGo4AnalysisDummySlot;
class TFolder;
class TGo4AnalysisObjectAccess;
class TGo4Status;
class TGo4Display;

enum EGo4ShellKind { Go4_sh = 0, Go4_rsh = 1, Go4_ssh = 2 };
enum EGo4ConsoleKind { Go4_qt = 1, Go4_xterm = 2, Go4_konsole = 3 };
enum EGo4AnalysisCodeKind { Go4_exe = 0, Go4_lib = 1 };

class TGo4AnalysisProxy : public TGo4ServerProxy {
   protected:
      Bool_t                    fIsServer{kFALSE};            //!
      TGo4AnalysisDummySlot    *fDummySlot{nullptr};          //!
      TGo4AnalysisObjectNames  *fAnalysisNames{nullptr};      //!
      TObjArray                 fxSubmittedProxy;             //!
      TGo4AnalysisObjectAccess *fxDefaultProxy{nullptr};      //!
      Bool_t                    fbNamesListReceived{kFALSE};  //!
      Bool_t                    fbAnalysisReady{kFALSE};      //!
      Bool_t                    fbAnalysisRunning{kFALSE};    //!
      Int_t                     fDisconectCounter{0};         //!
      TGo4Display              *fxDisplay{nullptr};           //!
      Int_t                     fActualRole{0};               //!
      TTimer                   *fxRefreshTimer{nullptr};      //!
      TTimer                   *fxConnectionTimer{nullptr};   //!
      Int_t                     fNumberOfWaitingProxyes{0};   //!

      TGo4AnalysisObjectAccess* FindSubmittedProxy(const char *pathname, const char *objname);
      void DeleteSubmittedProxy(TGo4AnalysisObjectAccess* proxy);

      void AssignNewNamesList(TGo4AnalysisObjectNames *objnames);

      void CallSlotUpdate();

   public:
      TGo4AnalysisProxy(Bool_t isserver = kFALSE);
      virtual ~TGo4AnalysisProxy();

      // proxy functionality

      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;

      Bool_t HasSublevels() const override;

      TGo4LevelIter *MakeIter() override;

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override;

      void WriteData(TGo4Slot *slot, TDirectory *dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory *dir) override;

      const char *GetContainedObjectInfo() override;
      Int_t GetObjectKind() const override { return TGo4Access::kndFolder; }
      const char *GetContainedClassName() const override { return ClassName(); }
      TObject *GetAssignedObject() override { return this; }

      void Update(TGo4Slot *slot, Bool_t strong) override;

      // server proxy functionality


      Bool_t IsGo4Analysis() const override { return kTRUE; }
      Bool_t IsAnalysisServer() const override { return fIsServer; }

      Bool_t IsConnected() const override;
      Bool_t IsViewer() const override;
      Bool_t IsController() const override;
      Bool_t IsAdministrator() const override;

      Bool_t RefreshNamesList() override;
      Bool_t DelayedRefreshNamesList(Int_t delay_sec) override;

      Bool_t CanSubmitObjects() const override { return IsConnected() && !IsViewer(); }
      void RequestAnalysisSettings() override;
      void SubmitAnalysisSettings() override;
      void CloseAnalysisSettings() override;

      Bool_t IsAnalysisRunning() const override { return fbAnalysisRunning; }
      void StartAnalysis() override;
      void StopAnalysis() override;

      Bool_t RequestObjectStatus(const char *objectname, TGo4Slot *tgtslot) override;

      void RequestEventStatus(const char *evname, Bool_t astree, TGo4Slot *tgtslot) override;

      Bool_t UpdateAnalysisObject(const char *objectname, TObject *obj) override;

      void ClearAllAnalysisObjects() override;
      void ClearAnalysisObject(const char *fullpath) override;

      void RemoteTreeDraw(const char *treename,
                          const char *varexp,
                          const char *cutcond,
                          const char *hname) override;

      void ChageObjectProtection(const char *fullpath, const char *flags) override;

      void RemoveObjectFromAnalysis(const char *fullpath) override;

      void ExecuteLine(const char *line) override;

      void PrintDynListEntry(const char *fullpath) override;

      void RemotePrintEvent(const char *evname,
                            Int_t evnumber,
                            Int_t subid,
                            Bool_t ishex,
                            Bool_t islong) override;

      void LoadConfigFile(const char *fname) override;
      void SaveConfigFile(const char *fname) override;

      void WriteAutoSave(const char *fname,
                         Int_t complevel,
                         Bool_t overwrite) override;

      Bool_t NamesListReceived() const override;

      // analysis proxy functionality

      TGo4Slot *UpdateObjectSlot();

      // communication with analysis functionality

      Int_t ConnectorPort();
      Int_t GetRole() const;

      void ReceiveObject(TNamed *obj);
      void ReceiveStatus(TGo4Status *status);
      Bool_t SubmitProxy(TGo4AnalysisObjectAccess *proxy);

      Int_t NumberOfWaitingProxyes() override { return fNumberOfWaitingProxyes; }

      Bool_t LaunchAsClient(TString &launchcmd,
                            TString &killcmd,
                            Int_t shellkind, // Go4_sh = 0, Go4_rsh = 1, Go4_ssh = 2
                            Int_t konsole,   // Go4_qt = 0, Go4_xterm = 1, Go4_konsole = 2
                            const char *name,
                            const char *remotehost,
                            const char *remotedir,
                            const char *remoteexe,
                            Int_t exe_kind = 0, //   Go4_exe = 0, Go4_lib = 1
                            const char *exeargs = nullptr);

      static Bool_t LaunchAsServer(TString &launchcmd,
                                   TString &killcmd,
                                   Int_t shellkind, // Go4_sh = 0, Go4_rsh = 1, Go4_ssh = 2
                                   Int_t konsole,   // Go4_qt = 0, Go4_xterm = 1, Go4_konsole = 2
                                   const char *name,
                                   const char *remotehost,
                                   Int_t remoteport, // needed for http server mode
                                   const char *remotedir,
                                   const char *remoteexe,
                                   Int_t exe_kind = 0,   //   Go4_exe = 0, Go4_lib = 1
                                   const char *exeargs = nullptr);

      Bool_t ConnectToServer(const char *remotehost,
                             Int_t remoteport,
                             Int_t ascontroller, // 0 - observer, 1 - controller, 2 - administrator
                             const char *accesspass);

      Bool_t WaitForConnection(Int_t seconds);

      void DisplayDeleted(TGo4Display *displ);

      void DisplayDisconnected(TGo4Display *displ);

      /**  Close connection to analysis and destroys proxy with
        *  correspondent slot. Wait waittime (in sec) for safe
        *  disconnection of the analysis.
        *  if servershutdown = kTRUE, shutdown command will be sent to analysis */
      void DisconnectAnalysis(Int_t waittime = 30, Bool_t servershutdown = kFALSE) override;

      Bool_t HandleTimer(TTimer *timer) override;

      /**  Set receiver for object envelopes, which coming from analysis without request  */
      void SetDefaultReceiver(TGo4ObjectManager *rcv, const char *path);

   ClassDefOverride(TGo4AnalysisProxy, 1);
};

#endif
