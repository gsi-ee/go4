// $Id$
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

/* Go4Fit Example N7
   Simultanious fit of two histogram
*/

#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TCollection.h"
#include "TApplication.h"

#include "../Go4Fit/TGo4FitMinuit.h"
#include "../Go4Fit/TGo4Fitter.h"
#include "../Go4Fit/TGo4FitDataHistogram.h"
#include "../Go4Fit/TGo4FitModelPolynom.h"
#include "../Go4Fit/TGo4FitModelGauss1.h"
#include "../Go4Fit/TGo4FitAxisTrans.h"
#include "../Go4Fit/TGo4FitLinearTrans.h"
#include "../Go4Fit/TGo4FitterOutput.h"

void Example7();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example7();

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
   TGo4Fitter *fitter = new TGo4Fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kFALSE);

// create object to fit for first histogram, but specify histogram later
   TGo4FitDataHistogram *data1 = new TGo4FitDataHistogram("data1",0);
   data1->SetUseBinScale(kTRUE);
   data1->SetRange(0,2.2,2.9);
   data1->SetNumberOfTransSlots(1);
   fitter->AddData(data1);

// create object to fit for second histogram, but specify histogram later
   TGo4FitDataHistogram *data2 = new TGo4FitDataHistogram("data2",0);
   data2->SetUseBinScale(kTRUE);
   data2->SetRange(0,2.2,2.9);
   data2->SetNumberOfTransSlots(1);
   fitter->AddData(data2);

   fitter->ConnectSlots("data1.Trans0","data2.Trans0");

   TGo4FitModel* gauss1 = new TGo4FitModelGauss1("Gauss1",2.553,0.015);
   gauss1->AssignToData("data1"); gauss1->AssignToData("data2",1.2);

   TGo4FitModel* gauss2 = new TGo4FitModelGauss1("Gauss2",2.672,0.015);
   gauss2->AssignToData("data1"); gauss2->AssignToData("data2",1.2);

   fitter->AddModel( "data1", new TGo4FitModelPolynom("Pol1_0",0.) );
   fitter->AddModel( "data1", new TGo4FitModelPolynom("Pol1_1",1.) );
   fitter->AddModel( "data2", new TGo4FitModelPolynom("Pol2_0",0.) );
   fitter->AddModel( "data2", new TGo4FitModelPolynom("Pol2_1",1.) );

   fitter->AddModel(gauss1);
   fitter->AddModel(gauss2);
   fitter->AddModel( "data2", new TGo4FitModelGauss1("Gauss3",2.597,0.014) );
   fitter->AddModel( "data2", new TGo4FitModelGauss1("Gauss4",2.717,0.014) );

// add ampl estimation action
   fitter->AddAmplEstimation(1);

// create minimizer and set it to fitter
   TGo4FitMinuit* fMinuit = new TGo4FitMinuit("Minuit");
   fMinuit->AddCommand("MIGRAD 500 1");
   fMinuit->AddCommand("MINOS 20");
   fitter->AddAction(fMinuit);

// set usage of buffers only for data objects, not for models
// this highly increase speed of evaluations
   fitter->SetMemoryUsage(1);

   return fitter;
}

// store fitter with all supplied objects
void StoreFitter(TGo4Fitter* fitter) {
    TFile *f = new TFile("Example7.root","recreate");
    fitter->Write("Fitter");
    delete f;
}

// read fitter from file
TGo4Fitter* RestoreFitter() {
    TFile *f = new TFile("Example7.root");
    TGo4Fitter* fitter = (TGo4Fitter*) f->Get("Fitter");
    delete f;
    return fitter;
}

void Example7() {
// create fitter
   TGo4Fitter* fitter = BuildFitter();

// store empty fitter to file and restore again
   StoreFitter(fitter);
   delete fitter;

   fitter = RestoreFitter();

// construct axis transformation object and set it for both data object
// slot of both data object was connected
   fitter->SetObject(ConstructTrans(), kTRUE);

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
   fitter->Print("Pars");
   fitter->Draw("#data1,Gauss1,Gauss2");
   fitter->Draw("#data2,Gauss1,Gauss2,Gauss3,Gauss4");

   delete fitter;
}
