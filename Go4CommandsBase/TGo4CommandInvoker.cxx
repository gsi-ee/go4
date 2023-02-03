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

#include "TGo4CommandInvoker.h"

#include "TMutex.h"
#include "TObjArray.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4CommandProtoList.h"
#include "TGo4RemoteCommand.h"

class TGo4Pair : public TObject {
  public:
     TGo4Pair(const char *name, TGo4CommandReceiver *p) :
        TObject(),
        fxName(name),
        fxReceiver(p)
     {
     }

     const char *GetName() const override { return fxName.Data(); }

     TGo4CommandReceiver *GetReceiver() const { return fxReceiver; }

  private:
     TString fxName;         //!
     TGo4CommandReceiver *fxReceiver{nullptr}; //!
};

TMutex *TGo4CommandInvoker::fxMutex = nullptr;
TGo4CommandInvoker *TGo4CommandInvoker::fxInstance = nullptr;
TGo4CommandProtoList *TGo4CommandInvoker::fxCommandList = nullptr;
TObjArray *TGo4CommandInvoker::fxArray = nullptr;

TGo4CommandInvoker::TGo4CommandInvoker() :
   TObject(),
   TGo4CommandReceiver()
{
   GO4TRACE((12,"TGo4CommandInvoker::TGo4CommandInvoker()", __LINE__, __FILE__));
   fxCommand = nullptr;
   fxArray = new TObjArray(10);
   fxMutex = new TMutex(kTRUE);
   fxCommandList = new TGo4CommandProtoList("Go4 base commandlist");
   Register("CommandInvoker",this); // we are ourselves receiver for remote commands
}

TGo4CommandInvoker::~TGo4CommandInvoker()
{
   GO4TRACE((12,"TGo4CommandInvoker::~TGo4CommandInvoker()", __LINE__, __FILE__));
   delete fxCommand;
   delete fxCommandList;
   delete fxMutex;
   fxArray->Delete();
   delete fxArray;
}

TGo4CommandInvoker *TGo4CommandInvoker::Instance()
{
   GO4TRACE((10,"TGo4CommandInvoker *TGo4CommandInvoker::Instance()", __LINE__, __FILE__));
   if (!fxInstance)
      fxInstance = new TGo4CommandInvoker();
   return fxInstance;
}

void TGo4CommandInvoker::Register(const char *name, TGo4CommandReceiver *p)
{
    GO4TRACE((12,"static void TGo4CommandInvoker::Register(const char *name, TGo4CommandReceiver *p)", __LINE__, __FILE__));
    TGo4LockGuard lockguard(fxMutex);
    fxArray->Add(new TGo4Pair(name, p));
}

void TGo4CommandInvoker::UnRegister(TGo4CommandReceiver *p)
{
    GO4TRACE((12,"static void TGo4CommandInvoker::UnRegister(TGo4CommandReceiver *p)", __LINE__, __FILE__));
    if (!fxArray) return;
    TGo4LockGuard lockguard(fxMutex);
    TIter riter(fxArray);
    while(auto ob = riter()) {
       TGo4Pair* pair = dynamic_cast<TGo4Pair*>(ob);
       if(!pair) {
          TGo4Log::Error("NEVER COME HERE: TGo4CommandInvoker::UnRegister - receiver list with no receiver");
          break;
       }
       if(pair->GetReceiver() == p) {
          fxArray->Remove(pair);
          delete pair;
          break;
       }
   }
   fxArray->Compress();
}

TGo4CommandReceiver *TGo4CommandInvoker::Lookup(const char *name)
{
   GO4TRACE((10,"static TGo4CommandReceiver *TGo4CommandInvoker::Lookup(const char *name)", __LINE__, __FILE__));
   TGo4Pair* pair = (TGo4Pair*) fxArray->FindObject(name);

   return pair ? pair->GetReceiver() : nullptr;
}

void TGo4CommandInvoker::Invoke(TGo4Command *com)
{
    GO4TRACE((12,"void TGo4CommandInvoker::Invoke(TGo4Command *com)", __LINE__, __FILE__));
    if(!com) return;
    TGo4LockGuard lockguard(fxMutex);
    TGo4CommandReceiver *rec = Lookup(com->GetReceiverName());
    if(rec) {
       com->SetReceiver(rec);
       if(com->GetMode()>=com->GetProtection())
           com->ExeCom();
       else
           com->RefuseCom();
    } else
       TGo4Log::Debug(" CommandInvoker: UNKNOWN receiver");
}

void TGo4CommandInvoker::Invoke()
{
   GO4TRACE((12,"void TGo4CommandInvoker::Invoke()", __LINE__, __FILE__));
   if(!fxCommand) return;
   TGo4LockGuard lockguard(fxMutex);

   TGo4CommandReceiver *rcv = Lookup(fxCommand->GetReceiverName());

   if(rcv) {
      fxCommand->SetReceiver(rcv);
      fxCommand->ExeCom();
   }
   delete fxCommand;
   fxCommand = nullptr;
}

void TGo4CommandInvoker::SetCommandList(TGo4CommandProtoList *list)
{
   delete fxCommandList;
   fxCommandList = list;
}

Int_t TGo4CommandInvoker::ExecuteFromRemote(TGo4RemoteCommand *remcom)
{
   if(!fxCommandList) return -1;
   TGo4Command *realcommand = fxCommandList->MakeCommand(remcom);
   realcommand->SetTaskName(remcom->GetTaskName());
   realcommand->SetMode(remcom->GetMode());
   Invoke(realcommand);
   delete realcommand;
   return 0;
}
