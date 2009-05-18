#include "TascaCaliGamma.h"

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
TascaCaliGamma::TascaCaliGamma() : TGo4Parameter(),
Recalibrate(kFALSE),
ReadLineFile(kFALSE),
LineFitter(0),
Calibrator(0),
fxCalibCurve(0),
fxCalibSpectrum(0)
{}
//***********************************************************
TascaCaliGamma::TascaCaliGamma(const char* name) : TGo4Parameter(name),
Recalibrate(kFALSE),
ReadLineFile(kFALSE),
LineFitter(0),
Calibrator(0),
fxCalibCurve(0),
fxCalibSpectrum(0)
{
    cout << "Tasca> TascaCaliGamma: " << name << " created" << endl;
    SaveToFile=kFALSE;
}
//***********************************************************
TascaCaliGamma::~TascaCaliGamma(){
	  delete LineFitter;
	  delete Calibrator;
    cout << "Tasca> TascaCaliGamma: " << GetName() << " deleted" << endl;
}
//***********************************************************

void TascaCaliGamma::Setup(Text_t *file,TH1* spectrum, TGraph* curve){
	fxCalibSpectrum=spectrum;
    fxSpectrumName=fxCalibSpectrum->GetName();
	fxCalibCurve=curve;
    fxGraphName=fxCalibCurve->GetName();
	// Set up fitters:
	LineFitter=new TGo4Fitter("Linefinder", TGo4Fitter::ff_least_squares, kTRUE);
    LineFitter->AddH1(GAMMA__DATA, fxCalibSpectrum, kFALSE);
	Calibrator=new TGo4Fitter("Calibrator", TGo4Fitter::ff_least_squares, kTRUE);
    Calibrator->AddGraph(GAMMA__GRAPH, fxCalibCurve, kFALSE);
	Calibrator->AddPolynomX(GAMMA__GRAPH,"A",__POLORDER__-1);

	for(Int_t ix=0;ix<GAMMA__LINES;++ix)
	{
	  fiLineChannel[ix]=0;
	  ffLineEnergy[ix]=0;
	}
	  LineFile=file;
	  ReadDatabase();
}
//-----------------------------------------------------------
Int_t TascaCaliGamma::PrintParameter(){
  return 0;
  cout << "CaliGamma " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaCaliGamma::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaCaliGamma")) {
    TascaCaliGamma * from = (TascaCaliGamma *) pp;
    for(UInt_t i=0;i<8;i++){
    	fdGammaE_a0[i]=from->fdGammaE_a0[i];
    	fdGammaE_a1[i]=from->fdGammaE_a1[i];
    	fdGammaE_a2[i]=from->fdGammaE_a2[i];
    	fdGammaT_a0[i]=from->fdGammaT_a0[i];
    	fdGammaT_a1[i]=from->fdGammaT_a1[i];
    	fdGammaT_a2[i]=from->fdGammaT_a2[i];
    }
    for(Int_t ord=0;ord<__POLORDER__;++ord)
     {
      fdA[ord]=from->fdA[ord];
     }
   Recalibrate=from->Recalibrate;
   ReadLineFile=from->ReadLineFile;
   if(LineFitter) delete LineFitter;
   LineFitter=from->LineFitter;
   from->LineFitter=0; // adopt lines finder
   if(Calibrator) delete Calibrator;
   Calibrator=from->Calibrator;
   from->Calibrator=0; // adopt calibration fitter
   LineFile=from->LineFile;

   // note: graph with calibration curve is not copied!


    for(Int_t ix=0;ix<GAMMA__LINES;++ix)
    {
       fiLineChannel[ix]=from->fiLineChannel[ix];
       ffLineEnergy[ix]=from->ffLineEnergy[ix];
       fxLineName[ix]=from->fxLineName[ix];
       //cout <<"updated line:"<<fxLineName[ix].Data() << endl;
    }
   cout <<"Updated Parameter:" << endl;
   //PrintParameter(0,0);
   // get references to graph and histogram from analysis:
   // note that updatefrom is only used on analysis side here!
   fxCalibCurve=dynamic_cast<TGraph*>(TGo4Analysis::Instance()->GetObject(fxGraphName.Data())) ;

   if(fxCalibCurve==0)
         cout <<"Graph "<<fxGraphName.Data() << " not existing in analysis"<< endl;
   else
          cout <<"Updated graph pointer ref to "<<fxCalibCurve << endl;

   // now reread database if desired:
   if(ReadLineFile)
      {
          cout <<"Reread database" << endl;
          ReadDatabase();
      }

   if(Recalibrate)
   {
      cout <<"Recalibrating..." << endl;
      // first we get the channels from the linesfinder fitter:

        for(Int_t i=0; i<GAMMA__LINES;++i)
        {
           const Text_t* linename=fxLineName[i];
           TGo4FitModel* mod=LineFitter->FindModel(linename);
           if(mod)
              {
                 // check here if component is active or not
                 if(mod->IsAssignTo(GAMMA__DATA)){
                     cout <<"find model "<<linename <<" line " << fiLineChannel[i] << endl;
                    fiLineChannel[i]=(Int_t) mod->GetParValue("Pos");
                 } else
                    fiLineChannel[i]=0; // mark not active lines
              }
           else
              {
                  cout <<"could not find model "<<linename << endl;
              }
        }


      // setup calibration graph with the new channel coords:
      if(fxCalibCurve)
         {
            fxCalibCurve->Set(0);
            Int_t point=0;
            for(Int_t ix=0;ix<GAMMA__LINES;++ix)
               {
               if(fiLineChannel[ix]!=0)
                  {
                     fxCalibCurve->SetPoint(point,
                                    fiLineChannel[ix],
                                    ffLineEnergy[ix]);
                     // we only fit active lines
                     ++point;
                  }
              } // for
        // now perform fit of calibration graph:
        Calibrator->SetObject(GAMMA__GRAPH, fxCalibCurve, kFALSE);
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
                 if(mod->IsAssignTo(GAMMA__GRAPH))
                    fdA[i]=mod->GetParValue("Ampl");
                 else
                    fdA[i]=0;
              }
           else
            ;//cout <<"could not find model "<<modname << endl;

        }

         }
      else
         {
          TGo4Log::Error("CaliGamma parameter %s has no TGraph!",
             GetName());
         }

   }

   cout << "Tasca> TascaCaliGamma: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}
void TascaCaliGamma::FitLines(TH1* histo, Double_t& a0, Double_t& a1, Double_t& a2){

}
void TascaCaliGamma::ReadDatabase()
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
TGo4Log::Info("CaliGamma energy file %s",LineFile.Data());
  Int_t ix=0;
  while(ix < GAMMA__LINES){
	do{
		  database.getline(nextline,__TEXTMAX__,'\n' ); // read whole line
		  if(database.eof() || !database.good()) break;
	  }while(strstr(nextline,"#") || strstr(nextline,"!") ); // skip any comments
	if(database.eof() || !database.good()) break;
	sscanf(nextline,"%s %f %d",buf,
		 &ffLineEnergy[ix],
		 &fiLineChannel[ix]);
	fxLineName[ix]=buf;
      cout <<"\tname:"<<fxLineName[ix].Data();
      cout <<"\te:"<<ffLineEnergy[ix];
      cout <<"\tch:"<<fiLineChannel[ix] << endl;

	LineFitter->AddGauss1(GAMMA__DATA,
							 fxLineName[ix].Data(),
							 fiLineChannel[ix],
							 TMath::Sqrt((Long_t) fiLineChannel[ix]));
	ix++;
  } // while(1)
  LineFitter->MemorizePars(); // Save start parameters
      //cout <<"scanned lines:"<<ix << endl;
}


