#include "TascaCalibPar.h"

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
TascaCalibPar::TascaCalibPar() :
   TGo4Parameter(),
   fbRecalibrate(kFALSE),
   fbReadDatabase(kFALSE),
   fxLinesFinder(0),
   fxCalibrator(0),
   fxCalibCurve(0),
   fxCalibSpectrum(0)
{
  fxDatabase="calilines.txt";
  Text_t buf[__TEXTMAX__];
  for(Int_t ord=0;ord<__POLORDER__;++ord) fdA[ord]=0;
  for(Int_t ix=0;ix<__LINESNUMBER__;++ix) {
     fiLinesChannel[ix]=0;
     ffLinesEnergy[ix]=0;
     snprintf(buf,__TEXTMAX__,"Defaultline-%d",ix);
     fxLinesNames[ix]=buf;
  }
}
//***********************************************************
TascaCalibPar::TascaCalibPar(const char* name, TH1* spectrum, TGraph* curve) :
   TGo4Parameter(name),
   fbRecalibrate(kFALSE),
   fbReadDatabase(kFALSE),
   fxLinesFinder(0),
   fxCalibrator(0),
   fxCalibCurve(curve),
   fxCalibSpectrum(spectrum)
{
// Set up fitters:
fxLinesFinder=new TGo4Fitter("Linefinder", TGo4Fitter::ff_least_squares, kTRUE);
fxCalibrator=new TGo4Fitter("Calibrator", TGo4Fitter::ff_least_squares, kTRUE);
if(fxCalibSpectrum)
   {
      fxLinesFinder->AddH1(__DATANAME__, fxCalibSpectrum, kFALSE);
      fxSpectrumName=fxCalibSpectrum->GetName();
   }
else
   {
      fxSpectrumName="Please specify calibration spectrum";
   }
if(fxCalibCurve)
   {
      fxCalibrator->AddGraph(__GRAPHNAME__, fxCalibCurve, kFALSE);
      fxGraphName=fxCalibCurve->GetName();
   }
else
   {
      fxSpectrumName="Please specify fit graph name";
   }
fxCalibrator->AddPolynomX(__GRAPHNAME__,"A",__POLORDER__-1);
// note that __POLORDER__ is number of polynom parameters here
// i.e. true order of polynom +1
Text_t modname[__TEXTMAX__];
for(Int_t i=0; i<__POLORDER__;++i)
        {
           fdA[i]=1/(i+1);
           snprintf(modname,__TEXTMAX__,"A_%d",i);
           TGo4FitModel* mod=fxCalibrator->FindModel(modname);
           if(mod)
              {
                 // for the beginning, disable models beyond order 1:
                 if(i>1) mod->ClearAssignmentTo(__GRAPHNAME__);
              }
           else
            cout <<"could not find model "<<modname << endl;
        }

for(Int_t ix=0;ix<__LINESNUMBER__;++ix)
{
  fiLinesChannel[ix]=0;
  ffLinesEnergy[ix]=0;
}
  fxDatabase="calilines.txt";
  ReadDatabase();
}
//***********************************************************
TascaCalibPar::~TascaCalibPar()
{
  delete fxLinesFinder;
  delete fxCalibrator;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaCalibPar::PrintParameter(Text_t * n, Int_t){


  return 0;
}
//-----------------------------------------------------------
Bool_t TascaCalibPar::UpdateFrom(TGo4Parameter *pp){
/////////////////////// under const /////////////////


  if(pp->InheritsFrom("TascaCalibPar"))
  {
    TascaCalibPar * from;
    from = (TascaCalibPar *) pp;
    for(Int_t ord=0;ord<__POLORDER__;++ord)
     {
      fdA[ord]=from->fdA[ord];
     }
   fbRecalibrate=from->fbRecalibrate;
   fbReadDatabase=from->fbReadDatabase;
   if(fxLinesFinder) delete fxLinesFinder;
   fxLinesFinder=from->fxLinesFinder;
   from->fxLinesFinder=0; // adopt lines finder
   if(fxCalibrator) delete fxCalibrator;
   fxCalibrator=from->fxCalibrator;
   from->fxCalibrator=0; // adopt calibration fitter

   // note: graph with calibration curve is not copied!


    for(Int_t ix=0;ix<__LINESNUMBER__;++ix)
    {
       fiLinesChannel[ix]=from->fiLinesChannel[ix];
       ffLinesEnergy[ix]=from->ffLinesEnergy[ix];
       fxLinesNames[ix]=from->fxLinesNames[ix];
       //cout <<"updated line:"<<fxLinesNames[ix].Data() << endl;
    }
   cout <<"Updated Parameter:" << endl;
   //PrintParameter(0,0);
   // get references to graph and histogram from analysis:
   // note that updatefrom is only used on analysis side here!
   fxCalibCurve=dynamic_cast<TGraph*>(TGo4Analysis::Instance()->GetObject(fxGraphName.Data())) ;

   if(fxCalibCurve==0)
      {
         cout <<"Graph "<<fxGraphName.Data() << " not existing in analysis"<< endl;
     }
   else
      {
          cout <<"Updated graph pointer ref to "<<fxCalibCurve << endl;
      }



   // now reread database if desired:
   if(fbReadDatabase)
      {
          cout <<"Reread database" << endl;
          ReadDatabase();
      }

   if(fbRecalibrate)
   {
      cout <<"Recalibrating..." << endl;
      // first we get the channels from the linesfinder fitter:

        for(Int_t i=0; i<__LINESNUMBER__;++i)
        {
           const Text_t* linename=fxLinesNames[i];
           TGo4FitModel* mod=fxLinesFinder->FindModel(linename);
           if(mod)
              {
                 // check here if component is active or not
                 if(mod->IsAssignTo(__DATANAME__))
                    fiLinesChannel[i]=(Int_t) mod->GetParValue("Pos");
                 else
                    fiLinesChannel[i]=0; // mark not active lines
              }
           else
              {
                  //cout <<"could not find model "<<linename << endl;
              }
        }


      // setup calibration graph with the new channel coords:
      if(fxCalibCurve)
         {
            fxCalibCurve->Set(0);
            Int_t point=0;
            for(Int_t ix=0;ix<__LINESNUMBER__;++ix)
               {
               if(fiLinesChannel[ix]!=0)
                  {
                     fxCalibCurve->SetPoint(point,
                                    fiLinesChannel[ix],
                                    ffLinesEnergy[ix]);
                     // we only fit active lines
                     ++point;
                  }
              } // for
        // now perform fit of calibration graph:
        fxCalibrator->SetObject(__GRAPHNAME__, fxCalibCurve, kFALSE);
        fxCalibrator->DoActions();
        fxCalibrator->PrintLines();
        // finally, copy results of calibration to the parameter fields:
        Text_t modname[__TEXTMAX__];
        for(Int_t i=0; i<__POLORDER__;++i)
        {
           snprintf(modname,__TEXTMAX__,"A_%d",i);
           TGo4FitModel* mod=fxCalibrator->FindModel(modname);
           if(mod)
              {
                 // check here if component is active or not
                 if(mod->IsAssignTo(__GRAPHNAME__))
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
          TGo4Log::Error("Calibration parameter %s has no TGraph!",
             GetName());
         }

   }

  }
else
     {
     cout << "Wrong parameter object: " << pp->ClassName() << endl;

     }
return kTRUE;
}

void TascaCalibPar::ReadDatabase()
{
// read energies from file:
Text_t nextline[__TEXTMAX__];
Text_t buf[__TEXTMAX__];
std::ifstream database(fxDatabase.Data());
if(database==0)
   {
      TGo4Log::Error("Open error of calibration energy file %s",
         fxDatabase.Data());
   }
else
   {
      Int_t ix=0;
      while(1){
          do{
              database.getline(nextline,__TEXTMAX__,'\n' ); // read whole line
              if(database.eof() || !database.good())
                {
                    break;
                }
              //cout <<"read line:"<<nextline << endl;
         }while(strstr(nextline,"#") || strstr(nextline,"!") ); // skip any comments
      if(database.eof() || !database.good()) break;
      sscanf(nextline,"%s %f %d",buf,
             &ffLinesEnergy[ix],
             &fiLinesChannel[ix]);
      fxLinesNames[ix]=buf;
//      cout <<"\tname:"<<fxLinesNames[ix].Data() << endl;
//      cout <<"\te:"<<ffLinesEnergy[ix] << endl;
//      cout <<"\tch:"<<fiLinesChannel[ix] << endl;

      fxLinesFinder->AddGauss1(__DATANAME__,
                                 fxLinesNames[ix].Data(),
                                 fiLinesChannel[ix],
                                 TMath::Sqrt((Long_t) fiLinesChannel[ix]));
      ix++;
      } // while(1)
      //cout <<"scanned lines:"<<ix << endl;

   } // if (database==0)


}



Double_t TascaCalibPar::Energy(Int_t channel)
{
  Double_t result=0;
  for(Int_t ord=0;ord<__POLORDER__; ++ord)
    {
         result+=fdA[ord]*TMath::Power(channel,ord);
    }
  return result;
}
