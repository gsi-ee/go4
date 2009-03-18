#include "TGo4Proxy.h"

#include "TROOT.h"

Bool_t TGo4Access::CanGetObject() const
{
   return kFALSE;
}

Bool_t TGo4Access::GetObject(TObject* &obj, Bool_t &owner) const
{
   obj = 0;
   owner = kFALSE;
   return kFALSE;
}


const char* TGo4Access::GetObjectName() const
{
   return 0;
}

TClass* TGo4Access::GetObjectClass() const
{
   const char* name = GetObjectClassName();
   return name==0 ? 0 : (TClass*) gROOT->GetListOfClasses()->FindObject(name);
}

const char* TGo4Access::GetObjectClassName() const
{
   return 0;
}

Int_t TGo4Access::AssignObjectTo(TGo4ObjectReceiver* rcv, const char* path)
{
   if ((rcv==0) || IsRemote()) return 0;
   TObject* obj = 0;
   Bool_t owner = kFALSE;
   if (!GetObject(obj, owner)) return 0;
   DoObjectAssignement(rcv, path, obj, owner);
   return 1;
}
