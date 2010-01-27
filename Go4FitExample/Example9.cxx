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

#ifndef __CINT__

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TApplication.h"

#include "TGo4FitMinuit.h"
#include "TGo4Fitter.h"
#include "TGo4FitDataHistogram.h"
#include "TGo4FitModelPolynom.h"
#include "TGo4FitModelGauss2.h"
#include "TGo4FitModelGaussN.h"


void Example9();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example9();

   theApp.Run();

   return 0;
}

#endif

void DrawHistogram(TH1* histo, const char* CanvasName, const char* DrawOption)
{
   TCanvas *fCanvas = new TCanvas(CanvasName,"Draw of histogram",3);
   fCanvas->cd();
   histo->Draw(DrawOption);
   fCanvas->Update();
}

void Example9()
{
   // create fitter
   TGo4Fitter *fitter = new TGo4Fitter("Fitter","Example fitter object");

   // create dummy histogram and set with ownership flag
   TH2D *histo = new TH2D("histo","dummy histogram",1000,0.,10.,1000,0.,10.);
   TGo4FitData* data = fitter->AddData(new TGo4FitDataHistogram("data",histo,kTRUE));
   data->SetRange(0,1.,3.); data->SetRange(0,7.,9.);
   data->SetRange(1,1.,3.); data->SetRange(1,7.,9.);

    // create models component and assign them to fitter
   TGo4FitModel* model1 = fitter->AddModel( "data", new TGo4FitModelGauss2("Gauss1",2.,8.,.5,.5,0.) );
   model1->SetAmplValue(1000.);

   TGo4FitModel* model2 = fitter->AddModel( "data", new TGo4FitModelGauss2("Gauss2",8.,8.,.5,.5,0.) );
   model2->SetAmplValue(1000.);

   TGo4FitModel* model3 = fitter->AddModel( "data", new TGo4FitModelGauss2("Gauss3",8.,2.,.5,.5,0.) );
   model3->SetAmplValue(1000.);

   TGo4FitModel* model4 = fitter->AddModel( "data", new TGo4FitModelGauss2("Gauss4",2.,2.,.5,.5,0.) );
   model4->SetAmplValue(1000.);


   // create result model without optimization, using initial parameters values
   TH1* res1 = (TH1*) fitter->CreateDrawObject("Large", "data", kTRUE);

   histo = new TH2D("histo2","dummy histogram",10,0.,10.,10,0.,10.);
   fitter->SetObject("data", histo, kTRUE);

   TH1* res2 = (TH1*) fitter->CreateDrawObject("Small", "data", kTRUE);

   model1->SetIntegrationsProperty(5);
   model2->SetIntegrationsProperty(5);
   model3->SetIntegrationsProperty(5);
   model4->SetIntegrationsProperty(5);

   TH1* res3 = (TH1*) fitter->CreateDrawObject("SmallI", "data", kTRUE);

   delete fitter;


 // draw modeled histogram
//   DrawHistogram(res1,"Canvas1","SURF1");           // takes too long time
   DrawHistogram(res2,"Canvas2","SURF1");
   DrawHistogram(res3,"Canvas3","SURF1");

   Double_t i1 = res1->Integral()/1000000.;
   Double_t i2 = res2->Integral()/100.;
   Double_t i3 = res3->Integral()/100.;

   cout << "Integral over 1000x1000 points         =  " << i1 << endl;
   cout << "Integral over 10x10 points             =  " << i2 << endl;
   cout << "Integral over 10x10 with model integr. =  " << i3 << endl << endl;

   cout << "Integral2/Integral1 = " << i2/i1 << endl;
   cout << "Integral3/Integral1 = " << i3/i1 << endl;
}
