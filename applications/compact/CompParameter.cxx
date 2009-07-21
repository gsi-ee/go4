//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompParameter.h"

#include "Riostream.h"

//***********************************************************
CompParameter::CompParameter() : TGo4Parameter() {}
//***********************************************************
CompParameter::CompParameter(const char* name) : TGo4Parameter(name),
Save(0),Fill(1),shift(5),EventStackSize(10),Printed(kFALSE)
{
    cout << "Comp> CompParameter: " << name << " created" << endl;
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
    AdcThreshold=100;
    AdcThresholdStop=100;
    AdcMax=3500;
    SystemTimeSecOff=1243462631;
}
//***********************************************************
CompParameter::~CompParameter(){
    //cout << "Comp> CompParameter: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t CompParameter::PrintParameter(){
if(!Printed){
  cout << "Comp> CompParameter " << GetName()<<":" <<endl;
  printf("      Timeoffset %d sec\n",SystemTimeSecOff);
  printf("      Adc:  80TofMin=%10d Thrs=%4d Stop=%4d Max=%4d\n",Adc80TofMin,AdcThreshold,AdcThresholdStop,AdcMax);
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
Bool_t CompParameter::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("CompParameter")) {
    CompParameter * from = (CompParameter *) pp;
    if(from->Save){
    }
    Fill=from->Fill;
    shift=from->shift;
    EventStackSize=from->EventStackSize;
   cout << "Comp> CompParameter: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

