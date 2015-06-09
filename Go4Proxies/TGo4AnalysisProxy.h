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
      Bool_t                    fIsServer;           //!
      TGo4AnalysisDummySlot*    fDummySlot;          //!
      TGo4AnalysisObjectNames*  fAnalysisNames;      //!
      TObjArray                 fxSubmittedProxy;    //!
      TGo4AnalysisObjectAccess* fxDefaultProxy;      //!
      Bool_t                    fbNamesListReceived; //!
      Bool_t                    fbAnalysisReady;     //!
      Bool_t                    fbAnalysisRunning;    //!
      Int_t                     fDisconectCounter;   //!
      TGo4Display*              fxDisplay;          //!
      TString                   fInfoStr;           //!
      Int_t                     fActualRole;        //!
      TTimer*                   fxRefreshTimer;     //!
      TTimer*                   fxConnectionTimer;     //!
      static Int_t              fNumberOfWaitingProxyes;  //!

      static Bool_t GetLaunchString(TString& launchcmd,
                                    TString& killcmd,
                                    Bool_t server,
                                    Int_t shellkind, // 0 - exec, 1 - rsh, 2 - ssh
                                    Int_t konsole,   // 1 - qtwindow, 2 - xterm, 3 - konsole
                                    const char* name,
                                    const char* remotehost,
                                    const char* remotedir,
                                    const char* remoteexe,
                                    Int_t guiport,
                                    Int_t exe_kind = 0, // 0 - executable, 1 - user library
                                    const char* exeargs = 0);

      TGo4AnalysisObjectAccess* FindSubmittedProxy(const char* pathname, const char* objname);
      void DeleteSubmittedProxy(TGo4AnalysisObjectAccess* proxy);

      void AssignNewNamesList(TGo4AnalysisObjectNames* objnames);

      void CallSlotUpdate();

   public:
      TGo4AnalysisProxy(Bool_t isserver = kFALSE);
      virtual ~TGo4AnalysisProxy();

      // proxy functionality

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);

      virtual Bool_t HasSublevels() const;

      virtual TGo4LevelIter* MakeIter();

      virtual TGo4Access* ProvideAccess(const char* name);

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual const char* GetContainedObjectInfo();
      virtual Int_t GetObjectKind() { return TGo4Access::kndFolder; }
      virtual const char* GetContainedClassName() { return ClassName(); }
      virtual TObject* GetAssignedObject() { return this; }

      virtual void Update(TGo4Slot* slot, Bool_t strong);

      // server proxy functionality

      virtual const char* GetServerName() const { return "Analysis"; }

      virtual Bool_t IsGo4Analysis() const { return kTRUE; }

      virtual Bool_t IsConnected();
      virtual Bool_t IsViewer();
      virtual Bool_t IsController();
      virtual Bool_t IsAdministrator();

      virtual Bool_t RefreshNamesList();
      virtual Bool_t DelayedRefreshNamesList(Int_t delay_sec);

      virtual Bool_t CanSubmitAnalysisSettings() { return IsConnected() && !IsViewer(); }
      virtual void RequestAnalysisSettings();
      virtual void SubmitAnalysisSettings();
      virtual void CloseAnalysisSettings();

      virtual Bool_t IsAnalysisRunning() const { return fbAnalysisRunning; }
      virtual void StartAnalysis();
      virtual void StopAnalysis();

      virtual Bool_t RequestObjectStatus(const char* objectname, TGo4Slot* tgtslot);

      virtual void RequestEventStatus(const char* evname, Bool_t astree, TGo4Slot* tgtslot);

      virtual Bool_t UpdateAnalysisObject(const char* objectname, TObject* obj);

      virtual void ClearAllAnalysisObjects();
      virtual void ClearAnalysisObject(const char* fullpath);

      virtual void RemoteTreeDraw(const char* treename,
                                  const char* varexp,
                                  const char* cutcond,
                                  const char* hname);

      virtual void ChageObjectProtection(const char* fullpath, const char* flags);

      virtual void RemoveObjectFromAnalysis(const char* fullpath);

      virtual void ExecuteLine(const char* line);

      virtual void PrintDynListEntry(const char* fullpath);

      virtual void RemotePrintEvent(const char* evname,
                                    Int_t evnumber,
                                    Int_t subid,
                                    Bool_t ishex,
                                    Bool_t islong);

      virtual void LoadConfigFile(const char* fname);
      virtual void SaveConfigFile(const char* fname);

      virtual void WriteAutoSave(const char* fname,
                                 Int_t complevel,
                                 Bool_t overwrite);

      /**  Close connection to analysis and destroys proxy with
        *  correspondent slot. Wait waittime (in sec) for safe
        *  disconnection of the analysis.
        *  if servershutdown = kTRUE, shutdown command will be sent to analysis */
      virtual void DisconnectAnalysis(Int_t waittime = 30, Bool_t servershutdown = kFALSE);

      // analysis proxy functionality

      TGo4Slot* LoginfoSlot();
      TGo4Slot* UpdateObjectSlot();

      // communication with analysis functionality

      Bool_t IsAnalysisServer() const { return fIsServer; }
      Int_t ConnectorPort();
      Int_t GetRole();

      void SetAnalysisReady(Bool_t on = kTRUE) { fbAnalysisReady = on; }
      Bool_t IsAnalysisReady() const { return fbAnalysisReady; }

      Bool_t NamesListReceived();

      void ReceiveObject(TNamed* obj);
      void ReceiveStatus(TGo4Status* status);
      Bool_t SubmitProxy(TGo4AnalysisObjectAccess* proxy);

      static Int_t NumberOfWaitingProxyes() { return fNumberOfWaitingProxyes; }

      Bool_t LaunchAsClient(TString& launchcmd,
                            TString& killcmd,
                            Int_t shellkind, // Go4_sh = 0, Go4_rsh = 1, Go4_ssh = 2
                            Int_t konsole,   // Go4_qt = 0, Go4_xterm = 1, Go4_konsole = 2
                            const char* name,
                            const char* remotehost,
                            const char* remotedir,
                            const char* remoteexe,
                            Int_t exe_kind = 0, //   Go4_exe = 0, Go4_lib = 1
                            const char* exeargs = 0);

      static Bool_t LaunchAsServer(TString& launchcmd,
                                   TString& killcmd,
                                   Int_t shellkind, // Go4_sh = 0, Go4_rsh = 1, Go4_ssh = 2
                                   Int_t konsole,   // Go4_qt = 0, Go4_xterm = 1, Go4_konsole = 2
                                   const char* name,
                                   const char* remotehost,
                                   const char* remotedir,
                                   const char* remoteexe,
                                   Int_t exe_kind = 0,   //   Go4_exe = 0, Go4_lib = 1
                                   const char* exeargs = 0);

      Bool_t ConnectToServer(const char* remotehost,
                             Int_t remoteport,
                             Int_t ascontroller, // 0 - observer, 1 - controller, 2 - administrator
                             const char* accesspass);

      Bool_t WaitForConnection(Int_t seconds);

      void DisplayDeleted(TGo4Display* displ);

      void DisplayDisconnected(TGo4Display* displ);

      virtual Bool_t HandleTimer(TTimer* timer);

      /**  Set receiver for object envelopes, which coming from analysis without request  */
      void SetDefaultReceiver(TGo4ObjectManager* rcv, const char* path);


   ClassDef(TGo4AnalysisProxy, 1);
};



#endif
