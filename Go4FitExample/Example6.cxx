/* Go4Fit Example N6
   Simultanious fit of two histogram
*/

#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "../Go4Fit/TGo4Fitter.h"
#include "../Go4Fit/TGo4FitDataHistogram.h"
#include "../Go4Fit/TGo4FitModelPolynom.h"
#include "../Go4Fit/TGo4FitModelGauss1.h"
#include "../Go4Fit/TGo4FitAxisTrans.h"
#include "../Go4Fit/TGo4FitLinearTrans.h"
#include "../Go4Fit/TGo4FitterConfig.h"
#include "TCollection.h"

void Example6();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example6();

   theApp.Run();

   return 0;
}

#endif

// routine to read histogram from examples file
TH1D* GetHistogram(const char* HistogramName) {
   TFile f1("histograms.root");
   TH1D* histo = (TH1D*) f1.Get(HistogramName);
   histo->SetDirectory(0);
   return histo;
}

// constrcut transformation object, which recalculate bin numbers to new scale values
// here simple linear transformation  is used
TGo4FitAxisTrans* ConstructTrans() {
   TGo4FitLinearTrans* trans = new TGo4FitLinearTrans("trans","linear axis transformation");
   trans->SetCoefByRange(3800,0.,3.8);
   return trans;
}

TGo4Fitter* BuildFitter() {
// create fitter and select function to fit
   TGo4Fitter *fitter = new TGo4Fitter("Fitter",TGo4Fitter::ff_ML_Poisson, kFALSE);

// create object to fit for first histogram, but specify histogram later
   TGo4FitDataHistogram *data1 = new TGo4FitDataHistogram("data1",0);
   data1->SetUseBinScale(kTRUE);
   data1->SetRange(0,2.2,2.9);
   fitter->AddData(data1);

// create four components for model of the first histogram
   fitter->AddModel( "data1", new TGo4FitModelPolynom("Pol1_0",0.) );
   fitter->AddModel( "data1", new TGo4FitModelPolynom("Pol1_1",1.) );
   fitter->AddModel( "data1", new TGo4FitModelGauss1("Gauss1_1",2.553,0.015) );
   fitter->AddModel( "data1", new TGo4FitModelGauss1("Gauss2_1",2.672,0.015) );

// create object to fit for second histogram, but specify histogram later
   TGo4FitDataHistogram *data2 = new TGo4FitDataHistogram("data2",0);
   data2->SetUseBinScale(kTRUE);
   data2->SetRange(0,2.2,2.9);
   fitter->AddData(data2);

// create six components for model of the first histogram
   fitter->AddModel( "data2", new TGo4FitModelPolynom("Pol2_0",0.) );
   fitter->AddModel( "data2", new TGo4FitModelPolynom("Pol2_1",1.) );
   fitter->AddModel( "data2", new TGo4FitModelGauss1("Gauss1_2",2.553,0.015) );
   fitter->AddModel( "data2", new TGo4FitModelGauss1("Gauss2_2",2.672,0.015) );
   fitter->AddModel( "data2", new TGo4FitModelGauss1("Gauss3_2",2.597,0.014) );
   fitter->AddModel( "data2", new TGo4FitModelGauss1("Gauss4_2",2.717,0.014) );

// specify init value and dependency between parameters
   TGo4FitterConfig* config = new TGo4FitterConfig("Config","fitter configuration");
   config->SetParInit("Pol1_0.Ampl",40);
   config->SetParInit("Pol1_1.Ampl",-0.01);
   config->SetParInit("Pol2_0.Ampl",500);
   config->SetParInit("Pol2_1.Ampl",-0.1);
   config->SetParDepend("Gauss1_2.Pos","Gauss1_1.Pos");
   config->SetParDepend("Gauss1_2.Width","Gauss1_1.Width");
   config->SetParDepend("Gauss2_2.Pos","Gauss2_1.Pos");
   config->SetParDepend("Gauss2_2.Width","Gauss2_1.Width");
   fitter->AddAction(config);

// add standard actions to fitter
   fitter->AddStandardActions();

// set usage of buffers only for data objects, not for models
// this highly increase speed of evaluations
   fitter->SetMemoryUsage(1);

   return fitter;
}

// store fitter with all supplied objects
void StoreFitter(TGo4Fitter* fitter) {
    TFile *f = new TFile("Example6.root","recreate");
    fitter->Write("Fitter");
    delete f;
}

// read fitter from file
TGo4Fitter* RestoreFitter() {
    TFile *f = new TFile("Example6.root");
    TGo4Fitter* fitter = (TGo4Fitter*) f->Get("Fitter");
    delete f;
    return fitter;
}

void Example6() {
// create fitter
   TGo4Fitter* fitter = BuildFitter();

// construct axis transformation object and set it for both data object, first will be owner
   TGo4FitAxisTrans* trans = ConstructTrans();
   fitter->FindData("data1")->AddAxisTrans(trans, kTRUE);
   fitter->FindData("data2")->AddAxisTrans(trans, kFALSE);

// assign histograms to fitter with ownership flag
   fitter->SetObject("data1", GetHistogram("hDeg120_P_c"), kTRUE);
   fitter->SetObject("data2", GetHistogram("hDeg120_CND"), kTRUE);

// do fit
   fitter->DoActions();

// store fitter to file and destroy it
   StoreFitter(fitter);
   delete fitter;


// restore fitter from file
// logically this is independent part and can be placed anywhere, just in another program
   fitter = RestoreFitter();

// show results of fitting
   fitter->PrintPars();
   fitter->Draw("#data1,Gauss1_1,Gauss2_1");
   fitter->Draw("#data2,Gauss1_2,Gauss2_2,Gauss3_2,Gauss4_2");

   delete fitter;
}

