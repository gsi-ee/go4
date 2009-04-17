#include "TascaParameter.h"

#include "Riostream.h"

//***********************************************************
TascaParameter::TascaParameter() : TGo4Parameter() {}
//***********************************************************
TascaParameter::TascaParameter(const char* name) : TGo4Parameter(name){
    cout << "Tasca> TascaParameter: " << name << " created" << endl;
    fbRestore=kFALSE;
    fbSave=kFALSE;
    fxFile="No file";
    for(UInt_t ix=0;ix<sizeof(ffPedestals)/sizeof(Float_t);++ix)
       ffPedestals[ix]=0;
}
//***********************************************************
TascaParameter::~TascaParameter(){
    cout << "Tasca> TascaParameter: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
void TascaParameter::SetPedestal(Int_t i, Float_t value){
	ffPedestals[i]=value;
	return;
}
//-----------------------------------------------------------
void TascaParameter::SavePedestal(){
    for(UInt_t ix=0;ix<sizeof(ffPedestals)/sizeof(Float_t);++ix)
       cout << ix << " " << ffPedestals[ix] << endl;
	return;
}
//-----------------------------------------------------------
Int_t TascaParameter::PrintParameter(Text_t * n, Int_t){
  return 0;
  cout << "Parameter " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaParameter::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaParameter")) {
    TascaParameter * from = (TascaParameter *) pp;
    if(from->fbRestore){

    }
    if(from->fbSave){
    	SavePedestal();
    }
    fxFile=from->fxFile;
   cout << "Tasca> TascaParameter: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

