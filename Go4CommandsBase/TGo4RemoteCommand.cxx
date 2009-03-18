#include "TGo4RemoteCommand.h"

#include "TGo4Log.h"
#include "TGo4CommandInvoker.h"

const char* TGo4RemoteCommand::fgxREMCOMEMPTYSTRING="--NOSTRING--";

TGo4RemoteCommand::TGo4RemoteCommand() :
   TGo4Command(),
   fxCommandName("THStop"),
   fxAggregate(0)
{
   InitPars();
}

TGo4RemoteCommand::TGo4RemoteCommand(const char* comname) :
   TGo4Command(),
   fxCommandName(comname),
   fxAggregate(0)
{
   InitPars();
}

void TGo4RemoteCommand::InitPars()
{
  SetName("BS_Remote");
  SetDescription("Container for remote commands");
  SetReceiverName("CommandInvoker");
  fiCommandID = 888;
  fiVersion = 3;
  fbIsSynchron = kFALSE;
  fbIsEnabled = kTRUE;
  fbIsExclusive = kFALSE;
  fbIsLocal = kFALSE;
  for(Int_t j=0;j<__REMCOMPARS__; j++) {
      fxParameter[j]=fgxREMCOMEMPTYSTRING;
      fiValue[j]=0;
   }
}

TGo4RemoteCommand::~TGo4RemoteCommand()
{
   //delete fxAggregate; // never delete aggregate owned by gui object manager!
}

Int_t TGo4RemoteCommand::ExeCom()
{
   TGo4CommandInvoker* inv = dynamic_cast<TGo4CommandInvoker*> (fxReceiverBase);
   if(inv==0) return -1;
   Int_t rev=inv->ExecuteFromRemote(this);
   delete fxAggregate; // only cleanup on executer side if necessary!
   return rev;
}

void TGo4RemoteCommand::SetAggregate(TObject* ob)
{
   delete fxAggregate;
   fxAggregate = ob;
}

TObject* TGo4RemoteCommand::GetAggregate(Bool_t chown)
{
   TObject* rev=fxAggregate;
   if(chown) fxAggregate=0; // change ownership to external client
   return rev;
}

void TGo4RemoteCommand::SetString(const char* txt,Int_t i)
{
   if ((i>=0) && (i<__REMCOMPARS__)) fxParameter[i]=txt;
}

const char* TGo4RemoteCommand::GetString(Int_t i) const
{
   return ((i>=0) && (i<__REMCOMPARS__)) ? fxParameter[i].Data() : 0;
}

void TGo4RemoteCommand::SetValue(Int_t num, Int_t i)
{
   if((i>=0) && (i<__REMCOMPARS__)) fiValue[i]=num;
}

Int_t TGo4RemoteCommand::GetValue(Int_t i) const
{
   return ((i>=0) && (i<__REMCOMPARS__)) ? fiValue[i] : 0;
}

const char* TGo4RemoteCommand::Get_fgxREMCOMEMPTYSTRING()
{
   return fgxREMCOMEMPTYSTRING;
}
