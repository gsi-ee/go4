//---------------------------------------------
// Go4 Comp analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompCaliFitter.h"

#include "TMath.h"
#include "TH1.h"
#include "TGraph.h"
#include "Riostream.h"
#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4Fitter.h"
#include "TGo4FitModel.h"
#include "TGo4Analysis.h"

//***********************************************************
CompCaliFitter::CompCaliFitter() : TGo4Parameter(),
LineFitter(0),
Calibrator(0),
fxCalibCurve(0),
fxCalibSpectrum(0)
{}
//***********************************************************
CompCaliFitter::CompCaliFitter(const char* name) : TGo4Parameter(name),
LineFitter(0),
Calibrator(0),
fxCalibCurve(0),
fxCalibSpectrum(0)
{
    cout << "Comp> CompCaliFitter: " << name << " created" << endl;
}
//***********************************************************
CompCaliFitter::~CompCaliFitter(){
	  delete LineFitter;
	  delete Calibrator;
    cout << "Comp> CompCaliFitter: " << GetName() << " deleted" << endl;
}
//***********************************************************

void CompCaliFitter::Setup(TGraph* curve){
	fxCalibCurve=curve;
    fxGraphName=fxCalibCurve->GetName();
    LineFile="alpha.txt";
	// Set up fitters:
	LineFitter=new TGo4Fitter("Linefinder", TGo4Fitter::ff_least_squares, kTRUE);
	Calibrator=new TGo4Fitter("Calibrator", TGo4Fitter::ff_least_squares, kTRUE);
    Calibrator->AddGraph(ADC__GRAPH, fxCalibCurve, kFALSE);
	Calibrator->AddPolynomX(ADC__GRAPH,"A",__POLORDER__-1);
	for(Int_t ix=0;ix<ADC__LINES;++ix)
	{
	  fiLineChannel[ix]=0;
	  ffLineEnergy[ix]=0;
	}
}
//-----------------------------------------------------------
Int_t CompCaliFitter::PrintParameter(){
  return 0;
  cout << "Comp> CaliFitter " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t CompCaliFitter::UpdateFrom(TGo4Parameter *pp){
if(!pp->InheritsFrom("CompCaliFitter")) {
	cout << "Wrong parameter object: " << pp->ClassName() << endl;
return kTRUE;
}
CompCaliFitter * from = (CompCaliFitter *) pp;
if(!from->DoFit)return kTRUE;

if(LineFitter) delete LineFitter;
LineFitter=from->LineFitter;
from->LineFitter=0; // adopt lines finder
if(Calibrator) delete Calibrator;
Calibrator=from->Calibrator;
from->Calibrator=0; // adopt calibration fitter
LineFile=from->LineFile;

// note: graph with calibration curve is not copied!


for(Int_t ix=0;ix<ADC__LINES;++ix)
{
	fiLineChannel[ix]=from->fiLineChannel[ix];
	ffLineEnergy[ix]=from->ffLineEnergy[ix];
	fxLineName[ix]=from->fxLineName[ix];
	//cout <<"updated line:"<<fxLineName[ix].Data() << endl;
}
//PrintParameter(0,0);
// get references to graph and histogram from analysis:
// note that updatefrom is only used on analysis side here!
fxCalibCurve=dynamic_cast<TGraph*>(TGo4Analysis::Instance()->GetObject(fxGraphName.Data())) ;

if(fxCalibCurve==0)
	cout <<"Graph "<<fxGraphName.Data() << " not existing in analysis"<< endl;

// now reread database if desired:
ReadDatabase();
cout << "Comp> CaliFitter: "<<"Calibrating..." << endl;
// first we get the channels from the linesfinder fitter:
Int_t lines=0;
for(Int_t i=0; i<ADC__LINES;++i)
{
	TGo4FitModel* mod=LineFitter->GetModel(i);
	if(mod)
	{
		// check here if component is active or not
		if(mod->GetParValue("Pos")>0)
			fiLineChannel[lines++]=(Int_t) mod->GetParValue("Pos");
	}
}
// setup calibration graph with the new channel coords:
fxCalibCurve->Set(0);
Int_t point=0;
for(Int_t ix=0;ix<lines;++ix)
{
	if(fiLineChannel[ix]!=0)
	{
		fxCalibCurve->SetPoint(point,
				fiLineChannel[ix],
				ffLineEnergy[ix]);
		cout<<"     "<<fiLineChannel[ix]<<" to "<<ffLineEnergy[ix]<<endl;
		// we only fit active lines
		++point;
	}
} // for
// now perform fit of calibration graph:
Calibrator->SetObject(ADC__GRAPH, fxCalibCurve, kFALSE);
Calibrator->DoActions();
Calibrator->PrintLines();
// finally, copy results of calibration to the parameter fields:
Text_t modname[__TEXTMAX__];
for(Int_t i=0; i<__POLORDER__;++i)
{
	snprintf(modname,__TEXTMAX__,"A_%d",i);
	TGo4FitModel* mod=Calibrator->FindModel(modname);
	if(mod)
	{
		// check here if component is active or not
		if(mod->IsAssignTo(ADC__GRAPH))
			fdA[i]=mod->GetParValue("Ampl");
		else
			fdA[i]=0;
	}
}
for(Int_t i=0;i<lines;i++){
	Double_t d=fdA[0]+fdA[1]*fiLineChannel[i]+fdA[2]*fiLineChannel[i]*fiLineChannel[i];
	cout <<"     "<< fiLineChannel[i]<<" to "<<ffLineEnergy[i]<<" = "<<d<<endl;

}


cout << "Comp> CaliFitter: fit done" << endl;
return kTRUE;
}
void CompCaliFitter::ReadDatabase()
{
// read energies from file:
Text_t nextline[__TEXTMAX__];
Text_t buf[__TEXTMAX__];
std::ifstream database(LineFile.Data());
if(database==0)
   {
      TGo4Log::Error("Open error of calibration energy file %s",
         LineFile.Data());
      return;
   }
cout<< "Comp> CaliFitter: energy file "<<LineFile.Data()<<endl;
  Int_t ix=0;
  while(1){
	do{
		  database.getline(nextline,__TEXTMAX__,'\n' ); // read whole line
		  if(database.eof() || !database.good()) break;
	  }while(strstr(nextline,"#") || strstr(nextline,"!") ); // skip any comments
	if(database.eof() || !database.good()) break;
	sscanf(nextline,"%s %f",buf,&ffLineEnergy[ix]);
	fxLineName[ix]=buf;
      cout <<"\tname: "<<fxLineName[ix].Data();
      cout <<"\te: "<<ffLineEnergy[ix] << endl;

	ix++;
  } // while(1)
}


