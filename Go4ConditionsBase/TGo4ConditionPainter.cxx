#include "TGo4ConditionPainter.h"

#include "TVirtualPad.h"
#include "TCutG.h"
#include "TH1.h"
#include "snprintf.h"

#include "TGo4Condition.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"

TGo4ConditionPainter::TGo4ConditionPainter(const Text_t * name, const Text_t * title)
: TGo4LabelPainter(name, title), fxCondition(0)
{

}

TGo4ConditionPainter::TGo4ConditionPainter(const Text_t * name)
: TGo4LabelPainter(name, "Go4 ConditionPainter"), fxCondition(0)
{

}

TGo4ConditionPainter::TGo4ConditionPainter()
: TGo4LabelPainter(), fxCondition(0)
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
   TH1* histogram=fxCondition->GetWorkHistogram();
   Text_t buffer[256];
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
               snprintf(buffer,256,"X1   = %.4E",fxCondition->GetXLow());
               AddToLabel(buffer);
               snprintf(buffer,256,"X2   = %.4E",fxCondition->GetXUp());
               AddToLabel(buffer);
               if(fxCondition->GetDimension()>1)
               {
                  snprintf(buffer,256,"Y1   = %.4E",fxCondition->GetYLow());
                  AddToLabel(buffer);
                  snprintf(buffer,256,"Y2   = %.4E",fxCondition->GetYUp());
                  AddToLabel(buffer);
               }
            }
         if(drint)
            {
               snprintf(buffer,256,"Int   = %.4E",fxCondition->GetIntegral(histogram));
               AddToLabel(buffer);
            }
         if(drxmean)
            {
               snprintf(buffer,256,"Xmean = %.4E",fxCondition->GetMean(histogram,1));
               AddToLabel(buffer);
            }

         if(drxrms)
            {
               snprintf(buffer,256,"Xrms  = %.4E",fxCondition->GetRMS(histogram,1));
               AddToLabel(buffer);
            }
         if(drxmax)
            {
               snprintf(buffer,256,"Xmax  = %.4E",fxCondition->GetXMax(histogram));
               AddToLabel(buffer);
            }
         if(drymean)
            {
               snprintf(buffer,256,"Ymean = %.4E",fxCondition->GetMean(histogram,2));
               AddToLabel(buffer);
            }
         if(dryrms)
            {
               snprintf(buffer,256,"Yrms  = %.4E",fxCondition->GetRMS(histogram,2));
               AddToLabel(buffer);
            }
         if(drymax)
            {
               snprintf(buffer,256,"Ymax  = %.4E",fxCondition->GetYMax(histogram));
               AddToLabel(buffer);
            }
         if(drcmax)
            {
               snprintf(buffer,256,"Cmax  = %.4E",fxCondition->GetCMax(histogram));
               AddToLabel(buffer);
            }
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


