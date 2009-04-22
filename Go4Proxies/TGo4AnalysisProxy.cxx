#include "TGo4AnalysisProxy.h"

#include "Riostream.h"
#include "TH1.h"
#include "TFolder.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TTime.h"
#include "TSystem.h"

#include "TGo4Log.h"

#include "TGo4Slot.h"
#include "TGo4ObjectProxy.h"
#include "TGo4ObjectManager.h"

#include "TGo4RemoteCommand.h"
#include "TGo4AnalysisObjectResult.h"
#include "TGo4ObjEnvelope.h"
#include "TGo4ComExecLine.h"

#include "TGo4Display.h"
#include "TGo4TaskManager.h"
#include "TGo4TaskHandler.h"
#include "TGo4ClientTask.h"
#include "TGo4ServerTask.h"
#include "TGo4BufferQueue.h"

#include "TGo4Parameter.h"
#include "TGo4ObjectStatus.h"

#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisClientStatus.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4TreeStructure.h"
#include "TGo4DynamicEntry.h"
#include "TGo4HistogramEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4BranchStatus.h"
#include "TGo4ParameterStatus.h"
#include "TGo4MemberStatus.h"

enum { cmdEnvelope, cmdStatus, cmdEvStatus };


class TGo4AnalysisObjectAccess : public TObject, public TGo4Access {
   public:

      TGo4AnalysisObjectAccess(TGo4AnalysisProxy* cont,
                              Int_t proxykind,
                              const char* name,
                              const char* classname,
                              const char* fullpath) :
          TObject(),
          TGo4Access(),
          fxAnalProxy(cont),
          fProxyKind(proxykind),
          fxObjName(name),
          fxObjClassName(classname),
          fxFullPath(fullpath),
          fxReceiver(0),
          fPermanentProxy(kFALSE)
      {
      }

      virtual ~TGo4AnalysisObjectAccess() {}

      virtual Bool_t IsRemote() const { return kTRUE; }
      virtual Bool_t CanGetObject() const { return kFALSE; }
      virtual const char* GetObjectName() const { return fxObjName.Data(); }
      virtual const char* GetObjectClassName() const { return fxObjClassName.Data(); }

      const char* GetPathName() const { return fxFullPath.Data(); }

      virtual Int_t AssignObjectTo(TGo4ObjectReceiver* rcv, const char* path)
      {
         if ((rcv==0) || (fxAnalProxy==0)) return 0;
         fxReceiver = rcv;
         fxReceiverPath = path;
         if (fxAnalProxy->SubmitProxy(this)) return 2;
         return 0;
      }

      Int_t ProxyKind() const { return fProxyKind; }

      void ChangeObjectName(const char* newname) { fxObjName = newname; }

      void SetSubmitTime() { fxSubmitTime = gSystem->Now(); }

      Bool_t CheckLifeTime(long int millisec)
      {
          long int res = gSystem->Now() - fxSubmitTime;
          return (res<0) || (res > millisec);
      }

      void ReceiveObject(TObject* obj, Bool_t owner)
      {
         DoObjectAssignement(fxReceiver, fxReceiverPath.Data(), obj, owner);
      }

   protected:
      TGo4AnalysisProxy*  fxAnalProxy;      //!
      Int_t                   fProxyKind;       //!
      TString                 fxObjName;        //!
      TString                 fxObjClassName;   //!
      TString                 fxFullPath;       //!
      TGo4ObjectReceiver*     fxReceiver;       //!
      TString                 fxReceiverPath;   //!
      TTime                   fxSubmitTime;     //!
      Bool_t                  fPermanentProxy;  //!
};


// ****************************************************************

class TGo4AnalysisLevelIter : public TGo4LevelIter {
   public:
      TGo4AnalysisLevelIter() :
         TGo4LevelIter(),
         fIter(0),
         fCurrent(0),
         fIsTree(kFALSE)
      {
      }

      TGo4AnalysisLevelIter(TFolder* folder, Bool_t istree = kFALSE) :
         TGo4LevelIter(),
         fIter(0),
         fCurrent(0),
         fIsTree(istree)
      {
         fIter = folder->GetListOfFolders()->MakeIterator();
      }

      virtual ~TGo4AnalysisLevelIter()
      {
         delete fIter;
      }

      virtual Bool_t next()
      {
         fCurrent = fIter->Next();
         return (fCurrent!=0);
      }

      virtual Bool_t isfolder()
      {
         return ((dynamic_cast<TFolder*>(fCurrent)!=0) ||
                 (dynamic_cast<TGo4TreeStructure*>(fCurrent)!=0));
      }

      virtual Int_t getflag(const char* flagname)
      {
         if (strcmp(flagname,"IsRemote")==0) return 1;
         if (strcmp(flagname,"IsDeleteProtect")==0) {
            TGo4ObjectStatus* sts = dynamic_cast<TGo4ObjectStatus*> (fCurrent);
            if (sts!=0){
              if (sts->IsDeleteProtect()) return 1;
                                     else return 0;
            }
         }
         if (strcmp(flagname,"IsResetProtect")==0) {
            TGo4ObjectStatus* sts = dynamic_cast<TGo4ObjectStatus*> (fCurrent);
            if (sts!=0){
              if (sts->IsResetProtect()) return 1;
                                    else return 0;
            }
         }

         return -1;
      }

      virtual Bool_t isremote()
      {
         return kTRUE;
      }

      virtual TGo4LevelIter* subiterator()
      {
         TGo4TreeStructure* tr = dynamic_cast<TGo4TreeStructure*>(fCurrent);
         if (tr!=0) return new TGo4AnalysisLevelIter(tr->GetNamesFolder(), kTRUE);
         return new TGo4AnalysisLevelIter((TFolder*)fCurrent, fIsTree);
      }

      virtual const char* name()
      {
         return fCurrent->GetName();
      }

      virtual const char* info()
      {
         return fCurrent->GetTitle();
      }

      virtual Int_t sizeinfo()
      {
         TGo4ObjectStatus* status =  dynamic_cast <TGo4ObjectStatus*> (fCurrent);
         return (status==0) ? 0 : status->GetObjectSize();
      }

      virtual Int_t GetKind()
      {
         Int_t kind = TGo4Access::kndNone;
         if (fIsTree)
           if (isfolder()) kind = TGo4Access::kndTreeBranch;
                      else kind = TGo4Access::kndTreeLeaf;
         else
           if (isfolder())
              kind = TGo4Access::kndFolder;
           else {
             kind = TGo4Access::kndObject;
             if ((fCurrent!=0) && fCurrent->InheritsFrom(TGo4ParameterStatus::Class()))
                kind = TGo4Access::kndGo4Param;
             else
             if ((fCurrent!=0) && fCurrent->InheritsFrom(TGo4MemberStatus::Class()))
                kind = TGo4Access::kndDataMember;
           }

         return kind;
      }

      virtual const char* GetClassName()
      {
         return EntryClassName(fCurrent);
      }

    static const char* EntryClassName(TObject* entry)
    {
       if (entry==0) return 0;

       if (entry->InheritsFrom(TGo4TreeStructure::Class()))
          return TTree::Class()->GetName();

       if(entry->InheritsFrom(TGo4BranchStatus::Class())) {
          TGo4BranchStatus* status = dynamic_cast <TGo4BranchStatus*> (entry);
          if (status!=0) return status->GetObjectClass();
       }

       if (entry->InheritsFrom(TGo4ParameterStatus::Class())) {
          TGo4ParameterStatus* status = dynamic_cast<TGo4ParameterStatus*> (entry);
          if (status!=0) return status->GetObjectClass();
       }

       if(entry->InheritsFrom(TGo4AnalysisStatus::Class())) {
           return TGo4AnalysisStatus::Class()->GetName();
       }

       if(entry->InheritsFrom(TGo4ObjectStatus::Class())) {
          TGo4ObjectStatus* status =  dynamic_cast <TGo4ObjectStatus*> (entry);
          if (status!=0) return status->GetObjectClass();
       }

       return entry->ClassName();
    }

   protected:
      TIterator*     fIter;     //!
      TObject*       fCurrent;  //!
      Bool_t         fIsTree;   //!
};

// ********************************************************************

Int_t TGo4AnalysisProxy::fNumberOfWaitingProxyes = 0;

TGo4AnalysisProxy::TGo4AnalysisProxy(Bool_t isserver) :
   TGo4Proxy(),
   fIsServer(isserver),
   fAnalysisNames(0),
   fxParentSlot(0),
   fxSubmittedProxy(),
   fbNamesListReceived(kFALSE),
   fbAnalysisReady(kFALSE),
   fbAnalysisSettingsReady(kFALSE),
   fbAnalysisRunning(kFALSE),
   fDisconectCounter(-111),
   fxDisplay(0),
   fInfoStr(),
   fActualRole(-1),
   fxRefreshTimer(0),
   fxConnectionTimer(0)
{
   fxDisplay = new TGo4Display(!isserver);

   fxDisplay->SetAnalysis(this);

   fxDisplay->GetTask()->Initialization();
}

TGo4AnalysisProxy::~TGo4AnalysisProxy()
{
//   Info("~TGo4AnalysisProxy","%x",this);

   if (fxRefreshTimer!=0) delete fxRefreshTimer;
   if (fxConnectionTimer!=0) delete fxConnectionTimer;

   fNumberOfWaitingProxyes -= fxSubmittedProxy.GetEntries();
   fxSubmittedProxy.Delete();

   delete fAnalysisNames;

   if (fxDisplay!=0) {
      // if no disconnect attempt was done, send at the end quit command
      // never delete display directly
      if (fDisconectCounter==-111)
        fxDisplay->SubmitCommand("MRQuit");

      if (fxDisplay!=0) {
         fxDisplay->SetAnalysis(0);
         TGo4Log::Debug("TGo4Display instance is not deleted correctly when analysis proxy is destroyed");
      }
   }
}

void TGo4AnalysisProxy::Initialize(TGo4Slot* slot)
{
   fxParentSlot = slot;

   TGo4Slot* subslot = new TGo4Slot(fxParentSlot, "Settings","Analsys configuration");
   subslot->SetProxy(new TGo4ObjectProxy());

   subslot = new TGo4Slot(fxParentSlot, "Ratemeter", "Analsys ratemeter");
   subslot->SetProxy(new TGo4ObjectProxy());

   subslot = new TGo4Slot(fxParentSlot, "Loginfo", "Analsys logging message");
   subslot->SetProxy(new TGo4ObjectProxy());

   subslot = new TGo4Slot(fxParentSlot, "UpdateObject", "Result of update object");
   subslot->SetProxy(new TGo4ObjectProxy());
}

TGo4Slot* TGo4AnalysisProxy::SettingsSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Settings");
}

TGo4Slot* TGo4AnalysisProxy::RatemeterSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Ratemeter");
}

TGo4Slot* TGo4AnalysisProxy::LoginfoSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Loginfo");
}

TGo4Slot* TGo4AnalysisProxy::UpdateObjectSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("UpdateObject");
}


void TGo4AnalysisProxy::Finalize(TGo4Slot* slot)
{
//   Info("Finalize","Parent %x %x", slot, fxParentSlot);

   slot->DeleteChilds();

   fxParentSlot = 0;
}

Bool_t TGo4AnalysisProxy::HasSublevels() const
{
   return (fAnalysisNames!=0);
}

TGo4LevelIter* TGo4AnalysisProxy::MakeIter()
{
   if (fAnalysisNames==0) return 0;

   return new TGo4AnalysisLevelIter(fAnalysisNames->GetNamesFolder(), kFALSE);
}

void TGo4AnalysisProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
}

void TGo4AnalysisProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
}

Int_t TGo4AnalysisProxy::GetRole()
{
   if (fxDisplay==0) return kGo4ComModeRefused;

   Int_t res = kGo4ComModeRefused;

   if (IsAnalysisServer()) {
     if (fActualRole>=0)
        res = fActualRole;
   } else
     if (fxDisplay->GetTask()!=0)
       if (fxDisplay->GetTask()->GetTaskHandler()!=0)
         res = fxDisplay->GetTask()->GetTaskHandler()->GetRole();
   return res;
}

Bool_t TGo4AnalysisProxy::IsViewer()
{
   return GetRole()==kGo4ComModeObserver;
}

Bool_t TGo4AnalysisProxy::IsController()
{
   return GetRole()==kGo4ComModeController;
}

Bool_t TGo4AnalysisProxy::IsAdministrator()
{
   return GetRole()==kGo4ComModeAdministrator;
}

const char* TGo4AnalysisProxy::GetContainedObjectInfo()
{
  switch (GetRole()) {
     case kGo4ComModeObserver: fInfoStr = "Observer"; break;
     case kGo4ComModeController: fInfoStr = "Controller"; break;
     case kGo4ComModeAdministrator: fInfoStr = "Administrator"; break;
     default: fInfoStr = "Disconnected"; break;
  }

   return fInfoStr.Data();
}

TGo4AnalysisObjectAccess* TGo4AnalysisProxy::FindSubmittedProxy(const char* pathname, const char* objname)
{
   for(int n=0;n<=fxSubmittedProxy.GetLast();n++) {
      TGo4AnalysisObjectAccess* proxy = (TGo4AnalysisObjectAccess*) fxSubmittedProxy.At(n);
      if (proxy==0) continue;
      if (strcmp(proxy->GetObjectName(), objname)!=0) continue;
      if ((pathname!=0) && (strcmp(proxy->GetPathName(), pathname)!=0)) continue;
      return proxy;
   }
   return 0;
}

void TGo4AnalysisProxy::DeleteSubmittedProxy(TGo4AnalysisObjectAccess* proxytodelete)
{
   bool docompress = false;
   if (proxytodelete!=0) {
      fxSubmittedProxy.Remove(proxytodelete);
      delete proxytodelete;
      docompress = true;
      fNumberOfWaitingProxyes--;
   }

   for(int n=0;n<=fxSubmittedProxy.GetLast();n++) {
      TGo4AnalysisObjectAccess* proxy = (TGo4AnalysisObjectAccess*) fxSubmittedProxy.At(n);
      if (proxy==0) continue;
      if (proxy->CheckLifeTime(100000)) {
         fxSubmittedProxy.Remove(proxy);
         docompress = true;
         delete proxy;
         fNumberOfWaitingProxyes--;
      }
   }

   if (docompress)
     fxSubmittedProxy.Compress();
}

void TGo4AnalysisProxy::ReceiveStatus(TGo4Status* status)
{
   if (dynamic_cast<TGo4AnalysisStatus*> (status)!=0) {
     SetAnalysisReady(kTRUE);
     if (SettingsSlot()!=0) {
        SettingsSlot()->AssignObject(status, kTRUE);
        status =0;
     }
     CallSlotUpdate();
   }

   TGo4AnalysisClientStatus* analstatus = dynamic_cast<TGo4AnalysisClientStatus*> (status);
   if (analstatus!=0) {
      fbAnalysisRunning = analstatus->IsAnalysisRunning();
      if (RatemeterSlot()!=0) {
         RatemeterSlot()->AssignObject(status, kTRUE);
         status=0;
      }
   }

   TGo4AnalysisObjectResult* obres=dynamic_cast<TGo4AnalysisObjectResult*> (status);
   if (obres!=0) {
        // put treatment of result object here
        // for the moment, we just check what we got:
//        cout <<"GOT result object in status channel: "<<obres->GetName() << endl;
//        obres->PrintStatus();
        TGo4AnalysisObjectNames* objnames = obres->GetNamesList(kTRUE);
        if (objnames!=0)
          AssignNewNamesList(objnames);
        if (UpdateObjectSlot()!=0) {
           TString fullname = obres->GetObjectFullName();

           if (fxParentSlot!=0) {
             fullname.Prepend("/");
             fullname.Prepend(fxParentSlot->GetName());
           }

           obres->SetObjectFullName(fullname.Data());

           UpdateObjectSlot()->AssignObject(obres, kTRUE);
           status=0;
        }
      CallSlotUpdate();
   }


   if (status!=0) {
     TString message = status->GetName();

//     cout << "Message = " <<  message << endl;

     if(message.Contains("event classes were initialized"))
       SetAnalysisSettingsReady(kTRUE);

     // for gui as client at analysis server
     if (message.Contains("is logged in at") && IsAnalysisServer())
        for(int j=0; j<4;++j)
           if(message.Contains(TGo4Command::GetModeDescription((Go4CommandMode_t) j ))) {
              fActualRole = j;
              break;
           }

     // this happens when analysis disconnects itself
     if (message.Contains("is shutting down now!")) {
         fActualRole = -1;
         TGo4Task* task = fxDisplay==0 ? 0 : fxDisplay->GetTask();
         TGo4BufferQueue* qu = task==0 ? 0 : task->GetCommandQueue();
         if(qu!=0) qu->Clear();
     }


     if (LoginfoSlot()!=0) {
        LoginfoSlot()->AssignObject(status, kTRUE);
        status = 0;
     }
   }

   if (status!=0) delete status;
}

void TGo4AnalysisProxy::ReceiveObject(TNamed* obj)
// object should be cleaned
{
   if (obj==0) return;
//   cout << " TGo4AnalysisProxy::ReceiveObject " << obj
//        << " name = " << obj->GetName()
//        << "  class  = " << obj->ClassName() << endl;

   TGo4ObjEnvelope* envelope = dynamic_cast<TGo4ObjEnvelope*> (obj);
   if (envelope!=0) {
      TGo4AnalysisObjectAccess* proxy = FindSubmittedProxy(envelope->GetObjFolder(), envelope->GetObjName());
      if (proxy!=0) {
         TObject* envelopeobj = envelope->TakeObject();
         if ((envelopeobj!=0) && envelopeobj->InheritsFrom(TH1::Class()))
            ((TH1*) envelopeobj)->SetDirectory(0);
         proxy->ReceiveObject(envelopeobj, kTRUE);
         DeleteSubmittedProxy(proxy);
      }
      delete envelope;
      return;
   }

   TGo4AnalysisObjectNames* objnames = dynamic_cast<TGo4AnalysisObjectNames*> (obj);
   if (objnames!=0) {
      AssignNewNamesList(objnames);
      return;
   }

   TGo4AnalysisObjectAccess* proxy = FindSubmittedProxy(0, obj->GetName());
   if (proxy!=0) {
      proxy->ReceiveObject(obj, kTRUE);
      DeleteSubmittedProxy(proxy);
      return;
   }

//   cout << "Object without receiver " << obj->GetName() << endl;

   delete obj;
}

void TGo4AnalysisProxy::AssignNewNamesList(TGo4AnalysisObjectNames* objnames)
{
   delete fAnalysisNames;
   fAnalysisNames = objnames;
   fbNamesListReceived = kTRUE;

   SetAnalysisReady(kTRUE);

   if (fxParentSlot!=0)
     fxParentSlot->ForwardEvent(fxParentSlot, TGo4Slot::evObjAssigned);
}

void TGo4AnalysisProxy::RefreshNamesList()
{
//   cout << " TGo4AnalysisProxy::RefreshNamesList() " << endl;
   //fxDisplay->SubmitCommand(new TGo4ComGetNamesList());
   fxDisplay->SubmitCommand("ANNames");
   fbNamesListReceived = kFALSE;
}

void TGo4AnalysisProxy::DelayedRefreshNamesList(Int_t delay_sec)
{
   if (fxRefreshTimer==0) fxRefreshTimer = new TTimer(this, 10, kTRUE);

   fxRefreshTimer->Start(delay_sec*1000, kTRUE);
}

Bool_t TGo4AnalysisProxy::NamesListReceived()
{
   return fbNamesListReceived;
}

TGo4Access* TGo4AnalysisProxy::MakeProxy(const char* name)
{
   if ((name==0) || (*name==0) || (fAnalysisNames==0)) return 0;

   TObject* entry = fAnalysisNames->GetNamesFolder()->FindObjectAny(name);

   const char* classname = TGo4AnalysisLevelIter::EntryClassName(entry);

   if (classname==0) return 0;

   TString objfolder, objname;
   TGo4Slot::ProduceFolderAndName(name, objfolder, objname);

//   cout << "Make TGo4AnalysisObjectAccess  name = " << objname
//        << "  class = " << classname << "  folder = " << objfolder << endl;

   return new TGo4AnalysisObjectAccess(this, cmdEnvelope, objname.Data(), classname, objfolder.Data());
}

void TGo4AnalysisProxy::Update(TGo4Slot* slot, Bool_t strong)
{
   if (strong)
     RefreshNamesList();
}


Bool_t TGo4AnalysisProxy::SubmitProxy(TGo4AnalysisObjectAccess* proxy)
{
   if (proxy==0) return kFALSE;

   fxSubmittedProxy.Add(proxy);

   fNumberOfWaitingProxyes++;

   proxy->SetSubmitTime();

   if (proxy->ProxyKind()==cmdEnvelope) {

      //TGo4ComGetEnvelope* com = new TGo4ComGetEnvelope(proxy->GetObjectName(), proxy->GetPathName());
      TGo4RemoteCommand* com = new TGo4RemoteCommand("ANGetEnvelope");
      com->SetString(proxy->GetObjectName(),0);
      com->SetString(proxy->GetPathName(),1);
      fxDisplay->SubmitCommand(com);
   } else
   if (proxy->ProxyKind()==cmdStatus) {

//      TGo4ComGetObjectStatus *com = new TGo4ComGetObjectStatus(proxy->GetObjectName());
//      com->SetFolderName(proxy->GetPathName());
      TGo4RemoteCommand* com= new TGo4RemoteCommand("ANObjectStatus");
      com->SetString(proxy->GetObjectName(),0);
      com->SetString(proxy->GetPathName(),1);
      TString rcvname = proxy->GetObjectName();
      rcvname.Append("_status");
      proxy->ChangeObjectName(rcvname.Data());
      fxDisplay->SubmitCommand(com);
   } else
   if (proxy->ProxyKind()==cmdEvStatus) {
//      TGo4ComGetCurrentEvent *com = new TGo4ComGetCurrentEvent(proxy->GetObjectName());
//      com->SetPrintoutOnly(kFALSE);
//      com->SetOutputEvent(kFALSE);
      Bool_t astree = strcmp(proxy->GetPathName(),"Tree")==0;
//      com->SetTreeMode(astree);
      TGo4RemoteCommand* com= new TGo4RemoteCommand("ANGetEvent");
      com->SetString(proxy->GetObjectName(),0);
      com->SetValue(kFALSE,0); // use output event
      com->SetValue(kFALSE,1); // printout only
      com->SetValue(astree,2); // treemode on or off
      fxDisplay->SubmitCommand(com);
   }

   return kTRUE;
}

void TGo4AnalysisProxy::RequestObjectStatus(const char* fullname, TGo4Slot* tgtslot)
{
   if ((fullname==0) || (tgtslot==0)) return;

   TString objfolder, objname;
   TGo4Slot::ProduceFolderAndName(fullname, objfolder, objname);

   TGo4AnalysisObjectAccess* proxy =
     new TGo4AnalysisObjectAccess(this, cmdStatus, objname, "", objfolder);

   TString tgtname;
   tgtslot->ProduceFullName(tgtname);
   proxy->AssignObjectTo(tgtslot->GetOM(), tgtname.Data());
}

void TGo4AnalysisProxy::RequestEventStatus(const char* evname, Bool_t astree, TGo4Slot* tgtslot)
{
   if (evname==0) return;

   TString folder, name;
   TGo4Slot::ProduceFolderAndName(evname, folder, name);

   if (tgtslot==0) {
//      TGo4ComGetCurrentEvent *com = new TGo4ComGetCurrentEvent(name);
//      com->SetPrintoutOnly(kTRUE);
//      com->SetOutputEvent(kFALSE);
//      com->SetTreeMode(astree);
      TGo4RemoteCommand* com= new TGo4RemoteCommand("ANGetEvent");
      com->SetString(name,0);
      com->SetValue(kFALSE,0); // use output event
      com->SetValue(kTRUE,1); // printout only
      com->SetValue(astree,2); // treemode on or off
      fxDisplay->SubmitCommand(com);
   } else {
      if (astree) folder="Tree";
            else folder="Event";
      TGo4AnalysisObjectAccess* proxy =
         new TGo4AnalysisObjectAccess(this, cmdEvStatus, name, "", folder);

      TString tgtname = tgtslot->GetFullName();
      proxy->AssignObjectTo(tgtslot->GetOM(), tgtname.Data());
   }
}


void TGo4AnalysisProxy::RemoteTreeDraw(const char* treename,
                                           const char* varexp,
                                           const char* cutcond,
                                           const char* hname)
{
   TString tfoldername, tobjectname;
   TGo4Slot::ProduceFolderAndName(treename, tfoldername, tobjectname);

   TString hfoldername, hobjectname;
   TGo4Slot::ProduceFolderAndName(hname, hfoldername, hobjectname);

   //fxDisplay->SubmitCommand(new TGo4ComAddTreeHistogram(hobjectname.Data(), tobjectname.Data(), varexp, cutcond));
   TGo4RemoteCommand* remcom= new TGo4RemoteCommand("ANAddTreeHis");
   remcom->SetString(hobjectname.Data(),0);
   remcom->SetString(tobjectname.Data(),1);
   remcom->SetString(varexp,2);
   remcom->SetString(cutcond,3);
   fxDisplay->SubmitCommand(remcom);
}

void TGo4AnalysisProxy::RemotePrintEvent(const char* evname,
                                         Int_t evnumber,
                                         Int_t subid,
                                         Bool_t ishex,
                                         Bool_t islong)
{
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANSetPrintEvent");
   com->SetString(evname,0);
   com->SetValue(evnumber, 0);
   com->SetValue(subid, 1);
   com->SetValue(islong, 2);
   com->SetValue(ishex, 3);
   com->SetValue(!ishex, 4);
   fxDisplay->SubmitCommand(com);
}

Bool_t TGo4AnalysisProxy::UpdateAnalysisObject(const char* fullpath, TObject* obj)
{
   if (obj==0) return kFALSE;

   const char* objname = obj->GetName();
   if ((objname==0) || (*objname==0)) return kFALSE;

   /// new with single set object commmand:
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANSetObject");
   com->SetString(objname,0); // objectname
   com->SetAggregate(obj); // parameter
   fxDisplay->SubmitCommand(com);

   return kTRUE;
}

void TGo4AnalysisProxy::LoadConfigFile(const char* fname)
{
   if((fname==0) || (strlen(fname)==0))
      fxDisplay->SubmitCommand("ANLoad");
   else {
      //TGo4ComLoadAnalysisStatus *com = new TGo4ComLoadAnalysisStatus(fname);
      TGo4RemoteCommand* com= new TGo4RemoteCommand("ANLoad");
      com->SetString(fname,0); // name of status file
      fxDisplay->SubmitCommand(com);
   }
}

void TGo4AnalysisProxy::SaveConfigFile(const char* fname)
{
   if((fname==0) || (strlen(fname)==0))
     fxDisplay->SubmitCommand("ANSave");
   else {
     //TGo4ComSaveAnalysisStatus *com = new TGo4ComSaveAnalysisStatus(fname);
     TGo4RemoteCommand* com= new TGo4RemoteCommand("ANSave");
     com->SetString(fname,0); // name of status file
     fxDisplay->SubmitCommand(com);
   }
}

void TGo4AnalysisProxy::WriteAutoSave(const char* fname,
                                          Int_t complevel,
                                          Bool_t overwrite)
{
//   TGo4ComAutosave *com = new TGo4ComAutosave();
//   com->SetAutoFileName(fname);
//   com->SetAutoSaveCompression(complevel);
//   com->SetAutoSaveOverwrite(overwrite);
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANAutosave");
   com->SetString(fname,0); // name of asf file
   com->SetValue(complevel,0); // file compression level
   com->SetValue(overwrite,1); // overwrite on or off
   fxDisplay->SubmitCommand(com);
}

void TGo4AnalysisProxy::StartAnalysis()
{
   fxDisplay->SubmitCommand("THStart");
   fbAnalysisRunning = kTRUE;
}

void TGo4AnalysisProxy::StopAnalysis()
{
   fxDisplay->SubmitCommand("THStop");
   fbAnalysisRunning = kFALSE;
}

void TGo4AnalysisProxy::ExecuteLine(const char* line)
{
   TGo4ComExecLine *com = new TGo4ComExecLine();
   com->SetLine(line);
   fxDisplay->SubmitCommand(com);
}

void TGo4AnalysisProxy::ClearAnalysisObject(const char* fullpath)
{
   TString foldername, objectname;
   TGo4Slot::ProduceFolderAndName(fullpath, foldername, objectname);
//   TGo4ComClearObject* com = new TGo4ComClearObject(objectname.Data());
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANClearObject");
   com->SetString(objectname.Data(),0);
   fxDisplay->SubmitCommand(com);
}

void TGo4AnalysisProxy::ChageObjectProtection(const char* fullpath, const char* flags)
{
   TString foldername, objectname;
   TGo4Slot::ProduceFolderAndName(fullpath, foldername, objectname);
//   TGo4ComSetProtections* com = new TGo4ComSetProtections(objectname.Data(), flags);
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANSetProtect");
   com->SetString(objectname.Data(),0);
   com->SetString(flags,2);
   fxDisplay->SubmitCommand(com);
}

void TGo4AnalysisProxy::RemoveObjectFromAnalysis(const char* fullpath, TClass* cl)
{
   TString foldername, objectname;
   TGo4Slot::ProduceFolderAndName(fullpath, foldername, objectname);
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANDelete");
   com->SetString(objectname.Data(),0);
   fxDisplay->SubmitCommand(com);
}

void TGo4AnalysisProxy::RequestAnalysisSettings()
{
//   TGo4ComGetAnalysisStatus* com = new TGo4ComGetAnalysisStatus;
//   fxDisplay->SubmitCommand(com);
   fxDisplay->SubmitCommand("ANGetStatus");
}

void TGo4AnalysisProxy::SubmitAnalysisSettings()
{
   TGo4AnalysisStatus* status = 0;
   if (SettingsSlot()!=0)
     status = dynamic_cast<TGo4AnalysisStatus*>(SettingsSlot()->GetAssignedObject());
   if (status!=0) {
      SetAnalysisSettingsReady(kFALSE);
      //TGo4ComSetAnalysisStatus *com = new TGo4ComSetAnalysisStatus(status);
      TGo4RemoteCommand* com= new TGo4RemoteCommand("ANSetStatus");
      com->SetAggregate(status);
      fxDisplay->SubmitCommand(com);
      fxDisplay->SubmitCommand("ANInit");
   }
}

void TGo4AnalysisProxy::PrintDynListEntry(const char* fullpath)
{
   TString foldername, objectname;
   TGo4Slot::ProduceFolderAndName(fullpath, foldername, objectname);
   TGo4RemoteCommand* com= new TGo4RemoteCommand("ANPrdyn");
   com->SetString(objectname.Data(),0);
   fxDisplay->SubmitCommand(com);
}

Int_t TGo4AnalysisProxy::ConnectorPort()
{
   if (fxDisplay==0) return 0;

   TGo4ServerTask* tsk = dynamic_cast<TGo4ServerTask*>
      (fxDisplay->GetTask());
   if (tsk==0) return 0;

   return tsk->GetTaskManager()->GetNegotiationPort();
}


Bool_t TGo4AnalysisProxy::LaunchAsClient(TString& launchcmd,
                                         TString& killcmd,
                                         Bool_t usessh,
                                         Int_t konsole,
                                         const char* name,
                                         const char* remotehost,
                                         const char* remotedir,
                                         const char* remoteexe)
{
   if (fxDisplay==0) return kFALSE;

   TGo4ServerTask* tsk = dynamic_cast<TGo4ServerTask*>
      (fxDisplay->GetTask());
   if (tsk==0) return kFALSE;

   Int_t guiport = tsk->GetTaskManager()->GetNegotiationPort();

   if (!GetLaunchString(launchcmd, killcmd,
                       kFALSE, usessh, konsole,
                       name, remotehost, remotedir, remoteexe,
                       guiport)) return kFALSE;

   tsk->StartConnectorThread();

   if ((konsole==2) || (konsole==3))
      gSystem->Exec(launchcmd.Data());

   return kTRUE;
}

Bool_t TGo4AnalysisProxy::LaunchAsServer(TString& launchcmd,
                                         TString& killcmd,
                                         Bool_t usessh,
                                         Int_t konsole,
                                         const char* name,
                                         const char* remotehost,
                                         const char* remotedir,
                                         const char* remoteexe)
{
   if (!GetLaunchString(launchcmd, killcmd,
                       kTRUE, usessh, konsole,
                       name, remotehost, remotedir, remoteexe,
                       0)) return kFALSE;

   if ((konsole==2) || (konsole==3))
      gSystem->Exec(launchcmd.Data());

   return kTRUE;
}

Bool_t TGo4AnalysisProxy::GetLaunchString(TString& launchcmd,
                                          TString& killcmd,
                                          Bool_t server,
                                          Bool_t usessh,
                                          Int_t konsole,
                                          const char* name,
                                          const char* remotehost,
                                          const char* remotedir,
                                          const char* remoteexe,
                                          Int_t guiport)
{
   const char* serverhost = gSystem->HostName();
   const char* sdisplay   = gSystem->Getenv("DISPLAY");
   const char* go4sys     = gSystem->Getenv("GO4SYS");
   const char* rootsys    = gSystem->Getenv("ROOTSYS");
   const char* path       = gSystem->Getenv("PATH");
   const char* ldpath     = gSystem->Getenv("LD_LIBRARY_PATH");

   if ((go4sys==0) || (strlen(go4sys)==0)) return kFALSE;

   TString filename = go4sys;
   filename+="/";
   filename += TGo4ServerTask::Get_fgcLAUNCHPREFSFILE();

   ifstream launchprefs(filename);
   if(!launchprefs) {
      TGo4Log::Debug("Master -- ERROR: Preferences file %s not existing, could not launch client ",
                  filename.Data());
      return kFALSE;
   }

   char formatstring[1000];

   if ((konsole<1) || (konsole>3)) konsole = 1;
   Int_t num = konsole;
   if (server) num+=3;

   for(int n=0;n<num;n++)
     launchprefs.getline(formatstring, 1000, '\n' );

   const char* shcom = usessh ? "ssh -X " : "rsh -n";
   TString serverdisplay = "";
   if (!usessh) {
      serverdisplay = "-display ";
      serverdisplay += sdisplay;
   }

   killcmd = "killall ";
   killcmd += remoteexe;

   if((strcmp(remotehost, gSystem->HostName())!=0) && (strcmp(remotehost,"localhost")!=0)) {
       TString precmd = shcom;
       precmd += " ";
       precmd += remotehost;
       precmd += " ";
       killcmd.Prepend(precmd);
   }

   launchcmd = "";

// formatstrings in etc/Go4LaunchClientPrefs.txt have been changed
// to pass go4sys:rootsys instead of rootsys (9 args only!!!)
// Accordingly, the startup scripts are changed to set GO4SYS and ROOTSYS from $1

   switch(konsole) {
     case 2: { // xterm
       launchcmd.Form(formatstring,
         shcom, remotehost, serverdisplay.Data(), name, remotehost, go4sys, go4sys, rootsys,
         path, ldpath, remotedir, remoteexe, name, serverhost, guiport, remotehost);
       break;
     }

     case 3: { // konsole
       launchcmd.Form(formatstring,
         shcom, remotehost, name, go4sys, go4sys, rootsys,
         path, ldpath, remotedir, remoteexe, name, serverhost, guiport, remotehost);
       break;
     }

     default: {  // Qt

      launchcmd.Form(formatstring,
         shcom, remotehost, go4sys, go4sys, rootsys,
         path, ldpath, remotedir, remoteexe, name, serverhost, guiport, remotehost);
      break;
     }
   }

   return kTRUE;
}

Bool_t TGo4AnalysisProxy::ConnectToServer(const char* remotehost,
                                          Int_t remoteport,
                                          Int_t ascontroller,
                                          const char* accesspass)
{

   TGo4ClientTask* client =
      dynamic_cast<TGo4ClientTask*> (fxDisplay->GetTask());
   if(client!=0) {
      Go4CommandMode_t mode = kGo4ComModeObserver;
      switch (ascontroller) {
         case 0: mode = kGo4ComModeObserver; break;
         case 1: mode = kGo4ComModeController; break;
         case 2: mode = kGo4ComModeAdministrator; break;
      }
      if (accesspass==0)
      switch (mode) {
         case kGo4ComModeObserver: accesspass = "go4view"; break;
         case kGo4ComModeController: accesspass = "go4ctrl"; break;
         case kGo4ComModeAdministrator: accesspass = "go4super"; break;
         default: accesspass = ""; break;
      }

//      cout << "mode = " << mode << "  pass = " << accesspass << endl;

      client->ConnectServer(remotehost, remoteport, mode, accesspass);
      RefreshNamesList();
   }

   return (client!=0);
}

Bool_t TGo4AnalysisProxy::WaitForConnection(Int_t seconds)
{
   Int_t period = seconds*1000;
   while (period>0) {
      if (IsConnected()) return kTRUE;
      gSystem->Sleep(100);
      gSystem->ProcessEvents();
      period-=100;
   }
   return IsConnected();
}


void TGo4AnalysisProxy::DisconnectAnalysis(Int_t waittime, Bool_t servershutdown)
{
   if (fDisconectCounter>0) return;

   if (fxDisplay!=0) {
      if (servershutdown)
         fxDisplay->SubmitCommand("SVQuit");
      else
         fxDisplay->SubmitCommand("MRQuit");
   }

   fDisconectCounter = waittime*10;

   fActualRole = -1;

   CallSlotUpdate();

   if (fxConnectionTimer==0)
      fxConnectionTimer = new TTimer(this, 10, kTRUE);

   fxConnectionTimer->Start(100, kTRUE);
}

Bool_t TGo4AnalysisProxy::IsConnected()
{
   if (fxDisplay==0) return kFALSE;

   if (!fxDisplay->IsConnected()) return kFALSE;

   return !IsAnalysisServer() || (fActualRole>=0);
}

void TGo4AnalysisProxy::DisplayDeleted(TGo4Display* displ)
{
   if (fxDisplay==displ) fxDisplay=0;
}

void TGo4AnalysisProxy::DisplayDisconnected(TGo4Display* displ)
{
   if ((fxDisplay==displ) && (displ!=0))
     DisconnectAnalysis(5, false);
//     fxDisplay->SubmitCommand("MRQuit");
}


void TGo4AnalysisProxy::CallSlotUpdate()
{
//   Info("CallSlotUpdate","ParentSlot = %x", fxParentSlot);

   if (fxParentSlot!=0)
      fxParentSlot->ForwardEvent(fxParentSlot, TGo4Slot::evObjUpdated);
}

Bool_t TGo4AnalysisProxy::HandleTimer(TTimer* timer)
{
   if (timer==fxRefreshTimer) {
      RefreshNamesList();
   } else

   if (timer==fxConnectionTimer) {
      if ((fxDisplay!=0) && (fDisconectCounter>0)) {
         fDisconectCounter--;
         fxConnectionTimer->Start(100, kTRUE);
      } else
      if (fxParentSlot!=0) {
         // this will also delete Analysis proxy itself
         // preactiacally the same as  delete fxParentSlot;
         fxParentSlot->Delete();
         //fxParentSlot = 0;
         //fxParentSlot->GetOM()->DeleteObject(fxParentSlot);
      }
         else delete this;
   }

   return kFALSE;
}
