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

#include "TGo4ConditionPainter.h"

#include "TVirtualPad.h"
#include "TCutG.h"
#include "TH1.h"

#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"

TGo4ConditionPainter::TGo4ConditionPainter()
: TGo4LabelPainter(), fxCondition(nullptr)
{
}

TGo4ConditionPainter::TGo4ConditionPainter(const char* name, const char* title)
: TGo4LabelPainter(name, title ? title : "Go4 ConditionPainter"), fxCondition(nullptr)
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
if(!gPad) return;
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
   TString fmt = fxCondition->GetLabelNumFormat();

   auto add_label = [this,&fmt](const char *name, Double_t value) {
      TString conv = name;
      while (conv.Length() < 5) conv.Append(" ");
      conv.Append(" = ");
      conv.Append(TString::Format(fmt.Data(), value));
      AddToLabel(conv.Data());
   };

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
          if(drlimits) {
              add_label("X1", fxCondition->GetXLow());
              add_label("X2", fxCondition->GetXUp());
              if(fxCondition->GetDimension()>1) {
                 add_label("Y1", fxCondition->GetYLow());
                 add_label("Y2", fxCondition->GetYUp());
              }
            }
         if(drint)
            add_label("Int", fxCondition->GetIntegral(histogram));
         if(drxmean)
            add_label("Xmean", fxCondition->GetMean(histogram, 1));
         if(drxrms)
            add_label("Xrms", fxCondition->GetRMS(histogram,1));
         if(drxmax)
            add_label("Xmax", fxCondition->GetXMax(histogram));
         if(drymean)
            add_label("Ymean", fxCondition->GetMean(histogram,2));
         if(dryrms)
            add_label("Yrms", fxCondition->GetRMS(histogram,2));
         if(drymax)
            add_label("Ymax", fxCondition->GetYMax(histogram));
         if(drcmax)
            add_label("Cmax", fxCondition->GetCMax(histogram));
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
   fxCondition = con;
}
