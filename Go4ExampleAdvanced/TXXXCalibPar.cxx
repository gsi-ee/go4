// $Id: TXXXCalibPar.cxx 478 2009-10-29 12:26:09Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXCalibPar.h"

#include "TMath.h"
#include "TH1.h"
#include "TGraph.h"
#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Fitter.h"
#include "TGo4FitModel.h"
#include "TGo4Analysis.h"

//***********************************************************
TXXXCalibPar::TXXXCalibPar() :
   TGo4Parameter(),
   fbRecalibrate(kFALSE),
   fbReadDatabase(kFALSE),
   fxLinesFinder(0),
   fxCalibrator(0),
   fxCalibCurve(0),
   fxCalibSpectrum(0)
{
   fxDatabase = "calilines.txt";
   for(Int_t ord=0;ord<__POLORDER__;++ord) fdA[ord]=0;
   for(Int_t ix=0;ix<__LINESNUMBER__;++ix) {
      fiLinesChannel[ix] = 0;
      ffLinesEnergy[ix] = 0;
      fxLinesNames[ix] = TString::Format("Defaultline-%d",ix);
   }
}
//***********************************************************
TXXXCalibPar::TXXXCalibPar(const char* name, TH1* spectrum, TGraph* curve) :
   TGo4Parameter(name),
   fbRecalibrate(kFALSE),
   fbReadDatabase(kFALSE),
   fxLinesFinder(0),
   fxCalibrator(0),
   fxCalibCurve(curve),
   fxCalibSpectrum(spectrum)
{
   // Set up fitters:
   fxLinesFinder = new TGo4Fitter("Linefinder", TGo4Fitter::ff_least_squares, kTRUE);
   fxCalibrator = new TGo4Fitter("Calibrator", TGo4Fitter::ff_least_squares, kTRUE);
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
   for(Int_t i=0; i<__POLORDER__;++i) {
      fdA[i]=1/(i+1);
      TString modname = TString::Format("A_%d",i);
      TGo4FitModel* mod = fxCalibrator->FindModel(modname.Data());
      if(mod) {
         // for the beginning, disable models beyond order 1:
         if(i>1) mod->ClearAssignmentTo(__GRAPHNAME__);
      } else
         TGo4Log::Error("could not find model %s", modname.Data());
   }

   for(Int_t ix=0;ix<__LINESNUMBER__;++ix) {
      fiLinesChannel[ix]=0;
      ffLinesEnergy[ix]=0;
   }
   fxDatabase="calilines.txt";
   ReadDatabase();
}
//***********************************************************
TXXXCalibPar::~TXXXCalibPar()
{
  delete fxLinesFinder;
  delete fxCalibrator;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXCalibPar::PrintParameter(Text_t * n, Int_t)
{
  return 0;
}
//-----------------------------------------------------------
Bool_t TXXXCalibPar::UpdateFrom(TGo4Parameter *source)
{
/////////////////////// under const /////////////////
   TXXXCalibPar * from = dynamic_cast<TXXXCalibPar*> (source);
   if (from==0) {
      TGo4Log::Error("Wrong parameter class: %s", source->ClassName());
      return kFALSE;
   }

   for(Int_t ord=0;ord<__POLORDER__;++ord)
      fdA[ord] = from->fdA[ord];
   fbRecalibrate = from->fbRecalibrate;
   fbReadDatabase = from->fbReadDatabase;
   if(fxLinesFinder) delete fxLinesFinder;
   fxLinesFinder = from->fxLinesFinder;
   from->fxLinesFinder = 0; // adopt lines finder
   if(fxCalibrator) delete fxCalibrator;
   fxCalibrator = from->fxCalibrator;
   from->fxCalibrator = 0; // adopt calibration fitter

   // note: graph with calibration curve is not copied!

   for(Int_t ix=0;ix<__LINESNUMBER__;++ix)
   {
      fiLinesChannel[ix]=from->fiLinesChannel[ix];
      ffLinesEnergy[ix]=from->ffLinesEnergy[ix];
      fxLinesNames[ix]=from->fxLinesNames[ix];
      //std::cout <<"updated line:"<<fxLinesNames[ix].Data() << std::endl;
   }
   std::cout <<"Updated Parameter:" << std::endl;
   //PrintParameter(0,0);
   // get references to graph and histogram from analysis:
   // note that updatefrom is only used on analysis side here!
   fxCalibCurve = dynamic_cast<TGraph*>(TGo4Analysis::Instance()->GetObject(fxGraphName.Data())) ;

   if(fxCalibCurve==0)
   {
      std::cout <<"Graph "<<fxGraphName.Data() << " not existing in analysis"<< std::endl;
   }
   else
   {
      std::cout <<"Updated graph pointer ref to "<<fxCalibCurve << std::endl;
   }

   // now reread database if desired:
   if(fbReadDatabase)
   {
      std::cout <<"Reread database" << std::endl;
      ReadDatabase();
   }

   if(fbRecalibrate)
   {
      std::cout <<"Recalibrating..." << std::endl;
      // first we get the channels from the linesfinder fitter:

      for(Int_t i=0; i<__LINESNUMBER__;++i)
      {
         const char* linename=fxLinesNames[i];
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
            //std::cout <<"could not find model "<<linename << std::endl;
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
         printf("fxCalibrator = %p\n", fxCalibrator);
         // finally, copy results of calibration to the parameter fields:
         for(Int_t i=0; i<__POLORDER__;++i) {
            TGo4FitModel* mod = fxCalibrator->FindModel(Form("A_%d",i));
            if(mod) {
               // check here if component is active or not
               if(mod->IsAssignTo(__GRAPHNAME__))
                  fdA[i] = mod->GetParValue("Ampl");
               else
                  fdA[i]=0;
            }
         }

      }
      else
      {
         TGo4Log::Error("Calibration parameter %s has no TGraph!",
               GetName());
      }
   }

  return kTRUE;
}

void TXXXCalibPar::ReadDatabase()
{
   // read energies from file:
   char nextline[__TEXTMAX__];
   char buf[__TEXTMAX__];
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
            //std::cout <<"read line:"<<nextline << std::endl;
         }while(strstr(nextline,"#") || strstr(nextline,"!") ); // skip any comments
         if(database.eof() || !database.good()) break;
         sscanf(nextline,"%s %f %d",buf,
               &ffLinesEnergy[ix],
               &fiLinesChannel[ix]);
         fxLinesNames[ix]=buf;
         //      std::cout <<"\tname:"<<fxLinesNames[ix].Data() << std::endl;
         //      std::cout <<"\te:"<<ffLinesEnergy[ix] << std::endl;
         //      std::cout <<"\tch:"<<fiLinesChannel[ix] << std::endl;

         fxLinesFinder->AddGauss1(__DATANAME__,
               fxLinesNames[ix].Data(),
               fiLinesChannel[ix],
               TMath::Sqrt((Long_t) fiLinesChannel[ix]));
         ix++;
      } // while(1)
      //std::cout <<"scanned lines:"<<ix << std::endl;

   } // if (database==0)
}



Double_t TXXXCalibPar::Energy(Int_t channel)
{
  Double_t result=0;
  for(Int_t ord=0;ord<__POLORDER__; ++ord)
    {
         result+=fdA[ord]*TMath::Power(channel,ord);
    }
  return result;
}
