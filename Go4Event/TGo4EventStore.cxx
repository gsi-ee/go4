#include "TGo4EventStore.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "Go4Exceptions.h"

TGo4EventStore::TGo4EventStore(const char* name)
:TNamed(name, "This is a Go4 Event Store")
{
TRACE((15,"TGo4EventStore::TGo4EventStore(Text_t*)",__LINE__, __FILE__));
}

TGo4EventStore::TGo4EventStore()
:TNamed("Default EventStore", "This is a Go4 Event Store")
{
TRACE((15,"TGo4EventStore::TGo4EventStore()",__LINE__, __FILE__));
}

TGo4EventStore::~TGo4EventStore()
{
TRACE((15,"TGo4EventStore::~TGo4EventStore()",__LINE__, __FILE__));
}

Int_t TGo4EventStore::Store(TGo4Parameter* cali)
{
   MayNotUse("TGo4EventStore::Store(TGo4Parameter* cali)");
   TGo4Log::Debug(" EventStore::Store Parameter not implemented for %s ",
         ClassName());
   return 0;
}


Int_t TGo4EventStore::Store(TGo4Condition* conny)
{
   MayNotUse("TGo4EventStore::Store(TGo4Condition* conny)");
   TGo4Log::Debug(" EventStore::Store Condition not implemented for %s ",
         ClassName());
   return 0;
}

Int_t TGo4EventStore::Store(TGo4Fitter* fitter)
{
   MayNotUse("TGo4EventStore::Store(TGo4Fitter* fitter)");
   TGo4Log::Debug(" EventStore::Store Fitter not implemented for %s ",
         ClassName());
   return 0;
}

Int_t TGo4EventStore::Store(TFolder* fold)
{
   MayNotUse("TGo4EventStore::Store(TFolder* folder)");
   TGo4Log::Debug(" EventStore::Store Folder not implemented for %s ",
         ClassName());
   return 0;
}


void TGo4EventStore::Clear(Option_t* opt)
{
 // dummy clear, may be implemented by user
 cout <<"default clear of eventstore "<<GetName() << endl;
}

