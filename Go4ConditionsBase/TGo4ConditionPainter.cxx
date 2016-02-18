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

#include "TGo4ConditionPainter.h"

#include "TVirtualPad.h"
#include "TCutG.h"
#include "TH1.h"
#include "snprintf.h"

#include "TGo4Condition.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"

TGo4ConditionPainter::TGo4ConditionPainter()
: TGo4LabelPainter(), fxCondition(0)
{
}

TGo4ConditionPainter::TGo4ConditionPainter(const char* name, const char* title)
: TGo4LabelPainter(name, title ? title : "Go4 ConditionPainter"), fxCondition(0)
{
}

TGo4ConditionPainter::~TGo4ConditionPainter()
{
   UnPaintCondition();
}



void TGo4ConditionPainter::PaintCondition(Option_t*)
{


}

void TGo4ConditionPainter::UnPaintCondition(Option_t*)
{

}

void TGo4ConditionPainter::PaintLabel(Option_t* opt)
{
if(gPad==0) return;
if(fxCondition && fxCondition->IsVisible())
   {
   //------ find out initial coordinates for labels near condition boundaries:
   Double_t xpmin=0;
   Double_t ypmin=0;
   TGo4WinCond* wconny=dynamic_cast<TGo4WinCond*>(fxCondition);
   TGo4PolyCond* pconny=dynamic_cast<TGo4PolyCond*>(fxCondition);
   if(wconny)
      {
         Double_t xpmax=0; Double_t ypmax=0; Int_t dim=0; // dummies here
         wconny->GetValues(dim, xpmin, xpmax, ypmin, ypmax);
         if(ypmin==0) ypmin=(gPad->GetUymax()-gPad->GetUymin())/2;
      }
   else if(pconny)
      {
          TCutG* cutg=pconny->GetCut(kFALSE);
          if(cutg) cutg->GetPoint(0,xpmin,ypmin);
      }
   else
      {
         xpmin=(gPad->GetUxmax()-gPad->GetUxmin())/2;
         ypmin=(gPad->GetUymax()-gPad->GetUymin())/2;
      }

   Bool_t haslabel=fxCondition->IsLabelDraw();
   Bool_t drlimits=fxCondition->IsLimitsDraw();
   Bool_t drint=fxCondition->IsIntDraw();
   Bool_t drxmean=fxCondition->IsXMeanDraw();
   Bool_t drxrms=fxCondition->IsXRMSDraw();
   Bool_t drymean=fxCondition->IsYMeanDraw();
   Bool_t dryrms=fxCondition->IsYRMSDraw();
   Bool_t drxmax=fxCondition->IsXMaxDraw();
   Bool_t drymax=fxCondition->IsYMaxDraw();
   Bool_t drcmax=fxCondition->IsCMaxDraw();
   Bool_t isarray=fxCondition->IsArrayType();
   Bool_t ismulti=fxCondition->IsMultiEdit();
   TString fmt=fxCondition->GetLabelNumFormat();
   TH1* histogram=fxCondition->GetWorkHistogram();
   if((isarray && !ismulti) || !haslabel)
      {
         // no label: remove previous label
          UnPaintLabel();
      }
   else
      {
          // at least one to be displayed: optionally create new label
          Double_t xoff=0.015*(gPad->GetUxmax()-gPad->GetUxmin());
          Double_t yoff=0.015*(gPad->GetUymax()-gPad->GetUymin()); // these are in pad coordinates (for logscale)
          SetX0( gPad->PadtoX( gPad->XtoPad(xpmin) + xoff));
          SetY0( gPad->PadtoY( gPad->YtoPad(ypmin) + yoff)); // initial coords are in "real" axis scale
          SetLineColor(fxCondition->GetLineColor());
          TString cap=fxCondition->GetName();
          TH1* his=fxCondition->GetWorkHistogram();
          if(his)
            {
               cap+=":";
               cap+=his->GetName();
            }
          SetCaption(cap.Data());
          TGo4LabelPainter::PaintLabel();// this creates new label at initial coords
          if(drlimits)
            {
               AddToLabel(Form(Form("X1   = %s",fmt.Data()),fxCondition->GetXLow()));
               AddToLabel(Form(Form("X2   = %s",fmt.Data()),fxCondition->GetXUp()));
               if(fxCondition->GetDimension()>1)
               {
                  AddToLabel(Form(Form("Y1   = %s",fmt.Data()),fxCondition->GetYLow()));
                  AddToLabel(Form(Form("Y2   = %s",fmt.Data()),fxCondition->GetYUp()));
               }
            }
         if(drint)
            AddToLabel(Form(Form("Int   = %s",fmt.Data()),fxCondition->GetIntegral(histogram)));
         if(drxmean)
            AddToLabel(Form(Form("Xmean = %s",fmt.Data()),fxCondition->GetMean(histogram,1)));
         if(drxrms)
            AddToLabel(Form(Form("Xrms  = %s",fmt.Data()),fxCondition->GetRMS(histogram,1)));
         if(drxmax)
            AddToLabel(Form(Form("Xmax  = %s",fmt.Data()),fxCondition->GetXMax(histogram)));
         if(drymean)
            AddToLabel(Form(Form("Ymean = %s",fmt.Data()),fxCondition->GetMean(histogram,2)));
         if(dryrms)
            AddToLabel(Form(Form("Yrms  = %s",fmt.Data()),fxCondition->GetRMS(histogram,2)));
         if(drymax)
            AddToLabel(Form(Form("Ymax  = %s",fmt.Data()),fxCondition->GetYMax(histogram)));
         if(drcmax)
            AddToLabel(Form(Form("Cmax  = %s",fmt.Data()),fxCondition->GetCMax(histogram)));
         RePaintLabel();
      }//if((isarray && !ismulti) || !haslabel)
   }//if(fxCondition && fxCondition->IsVisible())
else
   {
      UnPaintLabel();
   }
}

void TGo4ConditionPainter::SetCondition(TGo4Condition* con)
{
 fxCondition=con;
}


