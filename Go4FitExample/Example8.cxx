// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/**
 * Go4Fit Example8
 * @brief Use of range cut
 * @file Example8.cxx
 * @ingroup go4_fit
 */


#ifndef __CINT__

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TApplication.h"

#include "TGo4FitMinuit.h"
#include "TGo4Fitter.h"
#include "TGo4FitDataHistogram.h"
#include "TGo4FitModelPolynom.h"
#include "TGo4FitModelGauss2.h"
#include "TGo4FitModelGaussN.h"

void Example8();

int main(int argc, char **argv)
{
   TApplication theApp("Application", nullptr, nullptr);

   Example8();

   theApp.Run();

   return 0;
}

#endif

void DrawHistogram(TH1 *histo, const char *CanvasName, const char *DrawOption)
{
   TCanvas *fCanvas = new TCanvas(CanvasName,"Draw of histogram",3);
   fCanvas->cd();
   histo->Draw(DrawOption);
   fCanvas->Update();
}

void AddRangeCut(TGo4FitComponent *comp)
{
   Double_t x[] = { 2,3,8,7,2 };
   Double_t y[] = { 8,3,2,7,8 };
   TCutG *cut = new TCutG("cut1",5,x,y);
   comp->AddRangeCut(cut);
}

void Example8()
{
// create fitter
   TGo4Fitter *fitter = new TGo4Fitter("Fitter","Example fitter object");

// create dummy histogram and set with owneship flag
   TH2D *histo = new TH2D("histo","dummy histogram",100,0.,10.,100,0.,10.);
   fitter->AddData(new TGo4FitDataHistogram("data",histo,kTRUE));

// create models component and assign them to fitter
   TGo4FitModel *model = fitter->AddModel( "data", new TGo4FitModelGauss2("Gauss",5.,5.,1.,1.,-0.5) );
   model->SetAmplValue(1000.);
// only selected range will be used for modeling
   AddRangeCut(model);

// create result model without optimization, using initial parameters values
   TH1 *res = (TH1 *) fitter->CreateDrawObject("GaussModel","data",kTRUE);
   delete fitter;


// draw modeled histogram
   DrawHistogram(res,"Canvas1","SURF1");

// create new fitter
   fitter = new TGo4Fitter("Fitter2",TGo4Fitter::ff_ML_Poisson,kFALSE);

// use modeled histogram as data
   TGo4FitData *data = fitter->AddData(new TGo4FitDataHistogram("data",res));
// only selected range will be used for data extracting and modeling
   AddRangeCut(data);

// create N-dimensional gauss with 2 dimensions and set postion and width parameters
   model = fitter->AddModel( "data", new TGo4FitModelGaussN("Gauss",2) );
   model->SetPosition(0,4.5);
   model->SetPosition(1,4.5);
   model->SetWidth(0,1.7);
   model->SetWidth(1,1.7);
   model->SetAmplValue(500.);

   fitter->AddSimpleMinuit();

   fitter->SetMemoryUsage(1);

   fitter->DoActions();

   fitter->Print("Pars");

   delete fitter;
}
