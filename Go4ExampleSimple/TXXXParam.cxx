#include "TXXXParam.h"

#include "Riostream.h"

TXXXParam::TXXXParam(const char* name) : TGo4Parameter(name)
{
  fillRaw = kTRUE;
}

//***********************************************************
TXXXParam::~TXXXParam()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXParam::PrintParameter(Text_t *, Int_t)
{
   cout << "Parameter " << GetName() << ":" << endl;
   cout <<" fillRaw = " << (fillRaw ? "kTRUE" : "kFALSE") << endl;
   return 0;
}

//-----------------------------------------------------------
Bool_t TXXXParam::UpdateFrom(TGo4Parameter *source)
{
   TXXXParam* from = dynamic_cast<TXXXParam*>(source);
   if (from==0) {
      cout << "Wrong parameter class: " << source->ClassName() << endl;
      return kFALSE;
   }

   fillRaw = from->fillRaw;
   return kTRUE;
}
