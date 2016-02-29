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

#include "TGo4PolyCondPainter.h"

#include "TVirtualPad.h"
#include "TList.h"
#include "TROOT.h"

#include "TGo4PolyCondView.h"
#include "TGo4PolyCond.h"

TGo4PolyCondPainter::TGo4PolyCondPainter() :
   TGo4ConditionPainter(),
   fxCutView(0)
{
}

TGo4PolyCondPainter::TGo4PolyCondPainter(const char* name, const char* title) :
   TGo4ConditionPainter(name,title),
   fxCutView(0)
{
}

TGo4PolyCondPainter::~TGo4PolyCondPainter()
{
   UnPaintCondition();
   if (fxCutView!=0) {
     //std::cout <<"TGo4PolyCondPainter dtor deletes  cut view "<< (long)fxCutView << std::endl;
      delete fxCutView;
      fxCutView = 0;
   }
}


void TGo4PolyCondPainter::PaintCondition(Option_t* opt)
{
   if(gPad==0) return;
   TObject* cutinpad=gPad->GetListOfPrimitives()->FindObject(fxCutView);
   //std::cout<<"TGo4PolyCondPainter::PaintCondition with fxCutView "<<(long) fxCutView<<", gPad="<<(long) gPad<<", isatexecutemousevent="<<(fxCutView?fxCutView->IsAtExecuteMouseEvent():0)<<", cutinpad="<< (long)cutinpad;
   //std::cout<<", Threadid="<< (long) pthread_self()<< std::endl;

   //if(fxCutView && cutinpad && (fxCutView->IsAtExecuteMouseEvent() || fxCutView->IsAtExecuteMouseMenu())) return; // do not repaint during mouse move on polygon

   if(fxCutView && cutinpad && fxCutView->IsAtExecuteMouseEvent()) return; // do not repaint during mouse move on polygon
   if(fxCutView && fxCutView->IsAtExecuteMouseMenu()) return;

   TGo4PolyCond* pconny=dynamic_cast<TGo4PolyCond*>(fxCondition);
   if(pconny && pconny->IsVisible()) {
      TCutG* cutg=pconny->GetCut(kFALSE);
      if(cutg==0) return; // case of empty polygon condition
      // JAM2016: deletion from canvas is supressed inside Go4 by QtROOT interface. Just create if not already there:
      if(fxCutView==0) {
         // Only set up new view object if not already there
         // necessary to change cut interactively (points, colors, etc...)
         // Since cutg may be deleted from canvas by user.
         fxCutView = new TGo4PolyCondView(cutg);

         fxCutView->SetLineWidth(pconny->GetLineWidth());
         fxCutView->SetLineColor(pconny->GetLineColor());
         fxCutView->SetLineStyle(pconny->GetLineStyle());
         fxCutView->SetFillColor(pconny->GetFillColor());
         fxCutView->SetFillStyle(pconny->GetFillStyle());
      } else {
          //if(!fxCutView->IsAtExecuteMouseMenu()) // supress exchanging point arrays during insert/remove points menu JAM2016
           fxCutView->SetCut(cutg); // update view if condition was changed manually
        }

      fxCutView->SetCondition(pconny); // backreference for execute event
      if(!strcmp(opt,"fixstyle")) {
         // reproduce condition colors always
         // this mode will prevent the user from changing style interactively
         fxCutView->SetLineWidth(pconny->GetLineWidth());
         fxCutView->SetLineColor(pconny->GetLineColor());
         fxCutView->SetLineStyle(pconny->GetLineStyle());
         fxCutView->SetFillColor(pconny->GetFillColor());
         fxCutView->SetFillStyle(pconny->GetFillStyle());
      }
      if(cutinpad==0)
      {
        //std::cout <<"TGo4PolyCondPainter::PaintCondition appends to pad "<< (long)fxCutView << std::endl;
         fxCutView->AppendPad();
      }
   }// if(pconny && pconny->IsVisible())
else
   UnPaintCondition();
}

void TGo4PolyCondPainter::UnPaintCondition(Option_t* opt)
{
   if(fxCutView==0) return; // JAM2016
   gROOT->GetListOfCanvases()->RecursiveRemove(fxCutView);
   //std::cout <<"TGo4PolyCondPainter::UnPaintCondition removes  cut view "<< (long)fxCutView << std::endl;
   // we do not delete view, but restore graphics properties though invisible
   TString option(opt);
   if(option.Contains("reset")) {
     //std::cout <<"TGo4PolyCondPainter::UnPaintCondition with reset deletes cut view"<< (long)fxCutView << std::endl;
      delete fxCutView;
      fxCutView=0;
   }
}

void TGo4PolyCondPainter::DisplayToFront(Option_t* opt)
{
if(fxCutView) fxCutView->Pop(); // box to front
TGo4LabelPainter::DisplayToFront(opt); // label itself will be frontmost
}



void TGo4PolyCondPainter::SetCondition(TGo4Condition* con)
{
    fxCondition=con;
    if(fxCutView)
       fxCutView->SetCondition(dynamic_cast<TGo4PolyCond*>(con));
}
