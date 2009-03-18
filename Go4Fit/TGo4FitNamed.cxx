#include "TGo4FitNamed.h"
#include "Riostream.h"

TGo4FitNamed::TGo4FitNamed() : TNamed(), fxOwner(0), fxFullName() {
}

TGo4FitNamed::TGo4FitNamed(const char* Name, const char* Title, TNamed* Owner) :
    TNamed(Name, Title), fxOwner(Owner), fxFullName() {
}

TGo4FitNamed::~TGo4FitNamed() {
}

const char* TGo4FitNamed::GetOwnerFullName() {
   TGo4FitNamed* full = dynamic_cast<TGo4FitNamed*> (GetOwner());
   if(full) return full->GetFullName();
   if (GetOwner()) return GetOwner()->GetName();
              else return 0;

}

const char* TGo4FitNamed::GetFullName() {

  const char* ownname = GetOwnerFullName();

  if ((ownname!=0) && (strlen(ownname)>0)) {
    fxFullName = ownname;
    fxFullName += ".";
    fxFullName += GetName();
  } else fxFullName = GetName();
  return fxFullName.Data();
}

void TGo4FitNamed::Print(Option_t* option) const {
   TNamed::Print(option);
   const char* ownname = ((TGo4FitNamed*) this)->GetOwnerFullName();

   if ((ownname!=0) && (strlen(ownname)>0))
     cout << " Full name: " << ((TGo4FitNamed*) this)->GetFullName() << endl;
}
