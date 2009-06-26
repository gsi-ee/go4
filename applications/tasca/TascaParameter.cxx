//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaParameter.h"

#include "Riostream.h"

//***********************************************************
TascaParameter::TascaParameter() : TGo4Parameter() {}
//***********************************************************
TascaParameter::TascaParameter(const char* name) : TGo4Parameter(name),
Save(0),Fill(1),shift(5),EventStackSize(10),Printed(kFALSE)
{
    cout << "Tasca> TascaParameter: " << name << " created" << endl;
    AlphaTmin=0;
    AlphaTmax=180000000;
    Alpha1Tmin=0;
    Alpha1Tmax=180000000;
    Alpha2Tmin=0;
    Alpha2Tmax=180000000;
    Fission1Tmin=0;
    Fission1Tmax=180000000;
    Fission2Tmin=0;
    Fission2Tmax=180000000;
    AlphaMaxL=20000;
    AlphaMaxH=20000;
    AlphaMinL=0;
    AlphaMinH=0;
    Adc80TofMin=0;
    SystemTimeSecOff=1240000000;
}
//***********************************************************
TascaParameter::~TascaParameter(){
    //cout << "Tasca> TascaParameter: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaParameter::PrintParameter(){
if(!Printed){
  cout << "Tasca> TascaParameter " << GetName()<<":" <<endl;
  printf("      Adc:  80TofMin=%10d Thrs=%10d\n",Adc80TofMin,AdcThreshold);
  printf("      Alpha:    minL=%10.1f minH=%10.1f\n",AlphaMinL,AlphaMinH);
  printf("      Alpha:    maxL=%10.1f maxH=%10.1f\n",AlphaMaxL,AlphaMaxH);
  printf("      Alpha:    Tmin=%10d Tmax=%10d\n",AlphaTmin,AlphaTmax);
  printf("      Alpha:    Tmin=%10d Tmax=%10d\n",AlphaTmin,AlphaTmax);
  printf("      Alpha1:   Tmin=%10d Tmax=%10d\n",Alpha1Tmin,Alpha1Tmax);
  printf("      Alpha2:   Tmin=%10d Tmax=%10d\n",Alpha2Tmin,Alpha2Tmax);
  printf("      Fission1: Tmin=%10d Tmax=%10d\n",Fission1Tmin,Fission1Tmax);
  printf("      Fission2: Tmin=%10d Tmax=%10d\n",Fission2Tmin,Fission2Tmax);
}
  Printed=kTRUE;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaParameter::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaParameter")) {
    TascaParameter * from = (TascaParameter *) pp;
    if(from->Save){
    }
    Fill=from->Fill;
    shift=from->shift;
    EventStackSize=from->EventStackSize;
   cout << "Tasca> TascaParameter: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

