#include "TYYYParameter.h"

#include "Riostream.h"

#include "TGo4Fitter.h"

//***********************************************************
TYYYParameter::TYYYParameter() :
   TGo4Parameter("YYYParameter")
{
   frP1=0;
   frP2=0;

   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
   {
      fiDataArray[ix]=0;
      fxFitArray[ix]=0;
   }
}

//***********************************************************
TYYYParameter::TYYYParameter(const char* name) :
   TGo4Parameter(name)
{
   frP1=10;
   frP2=20;
   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
   {
      fiDataArray[ix]=ix+30;
      fxFitArray[ix] = new TGo4Fitter();
   }
}
//***********************************************************
TYYYParameter::~TYYYParameter()
{
   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
     if(fxFitArray[ix]) delete fxFitArray[ix];
}
//***********************************************************

//-----------------------------------------------------------
Int_t TYYYParameter::PrintParameter(Text_t * n, Int_t)
{
  cout << "Parameter " << GetName()<<":" <<endl;
  cout << " P1="<<frP1<<endl;
  cout << " P2="<<frP2<< endl;
  for(Int_t ix=0;ix<__ARRAYSIZE__;++ix) {
     cout << "fiDataArray["<<ix<<"]="<<fiDataArray[ix]<<endl;
     if (fxFitArray[ix]) fxFitArray[ix]->PrintLines();
  }
  return 0;
}
//-----------------------------------------------------------
Bool_t TYYYParameter::UpdateFrom(TGo4Parameter *pp)
{
   TYYYParameter* from = dynamic_cast<TYYYParameter*> (pp);
   if (from==0) {
      cout << "Wrong parameter class: " << pp->ClassName() << endl;
      return kFALSE;
   }

   cout << "Parameter " << GetName() << ": P1=" << frP1 << " P2="<< frP2 << endl;
   frP1=from->frP1;
   frP2=from->frP2;
   for(Int_t ix=0;ix<__ARRAYSIZE__;++ix)
   {
      fiDataArray[ix] = from->fiDataArray[ix];
      // replace old fitters by copy of source fitter:
      if(fxFitArray[ix]) {
         delete fxFitArray[ix];
         fxFitArray[ix] = 0;
      }
      if (from->fxFitArray[ix])
         fxFitArray[ix] = (TGo4Fitter*) from->fxFitArray[ix]->Clone();
   }
   return kTRUE;
}
