// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4WinCondView.h"

#include "RVersion.h"
#include "Buttons.h"
#include "TPad.h"

#include "TGo4WinCond.h"

TGo4WinCondView::TGo4WinCondView(Double_t x1,Double_t y1,Double_t x2,Double_t y2)
   :TBox(x1,y1,x2,y2), fbExecutesMouseEvent(kFALSE), fxWinCondition(0)
{
    SetBit(kMustCleanup);
    //SetBit(kCanDelete, kFALSE);
}

TGo4WinCondView::TGo4WinCondView()
   : TBox(),fbExecutesMouseEvent(kFALSE),fxWinCondition(0)
{
    SetBit(kMustCleanup);
    //SetBit(kCanDelete, kFALSE);
}

TGo4WinCondView::~TGo4WinCondView()
{
}


void TGo4WinCondView::Paint(Option_t* opt)
{
if(fxWinCondition) TBox::Paint(opt);
// we have a valid condition set from painter. Otherwise,
// suppress painting of condition view without owner. This case
// happens after insert canvas in go4 (separate cloning
// of lists of primitive members), thus leading to a
// second "ghost condition box"

}
void TGo4WinCondView::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
// note: This method is never called in ROOT>=4.03/02 when
// logscale is enabled on pad.
// So no mouse modifications of condition possible
// TO BE INVESTIGATED!  JA
if(gPad==0) return;

if(event==kButton1Down && fxWinCondition)
{
  fbExecutesMouseEvent=kTRUE; // only lock painting if we really touch the box JAM
}

TBox::ExecuteEvent(event,px,py);
if(event==kButton1Up && fxWinCondition)
   {
   Pop(); // do condition under edit into foreground, for condarray
   fxWinCondition->ResetLabel("pop"); // always pop our label to foreground
   // here we update the condition values if necessary:
   Double_t epsilon=fxWinCondition->GetUpdateEpsilon();
   Double_t xmin=0;
   Double_t xmax=0;
   Double_t ymin=0;
   Double_t ymax=0;
   Int_t dim=0;
   fxWinCondition->GetValues(dim,xmin,xmax,ymin, ymax);

   // note: newer root versions treat log scale correctly in TBox
#if ROOT_VERSION_CODE < ROOT_VERSION(4,3,2)
   Double_t X1 = gPad->PadtoX(GetX1()); // regard the log scale!
   Double_t X2 = gPad->PadtoX(GetX2());
   Double_t Y1 = gPad->PadtoY(GetY1());
   Double_t Y2 = gPad->PadtoY(GetY2());
#else
   Double_t X1 = GetX1();
   Double_t X2 = GetX2();
   Double_t Y1 = GetY1();
   Double_t Y2 = GetY2();
#endif
   if(dim>1)
         {
            // check if update is needed:
           if((TMath::Abs(xmin-X1) > epsilon) ||
               (TMath::Abs(xmax-X2) > epsilon) ||
               (TMath::Abs(ymin-Y1) > epsilon) ||
               (TMath::Abs(ymax-Y2) > epsilon) )
               {
                  //std::cout <<"ExecuteEvent modified 2d condition with colors, epsilon="<<epsilon << std::endl;
                  fxWinCondition->SetValues(X1, X2, Y1, Y2 );
                  fxWinCondition->SetLineColor(GetLineColor());
                  fxWinCondition->SetLineWidth(GetLineWidth());
                  fxWinCondition->SetLineStyle(GetLineStyle());
                  fxWinCondition->SetFillColor(GetFillColor());
                  fxWinCondition->SetFillStyle(GetFillStyle());
                  fxWinCondition->SetChanged(kTRUE);
               }
         }
      else
         {
            if((TMath::Abs(xmin-X1) > epsilon) ||
               (TMath::Abs(xmax-X2) > epsilon) )
               {
                  //std::cout <<"ExecuteEvent modified 1d condition with colors, epsilon="<<epsilon << std::endl;
                  fxWinCondition->SetValues(X1, X2);
                  fxWinCondition->SetLineColor(GetLineColor());
                  fxWinCondition->SetLineWidth(GetLineWidth());
                  fxWinCondition->SetLineStyle(GetLineStyle());
                  fxWinCondition->SetFillColor(GetFillColor());
                  fxWinCondition->SetFillStyle(GetFillStyle());
                  fxWinCondition->SetChanged(kTRUE);
               }
         }// if(dim>1)
   fbExecutesMouseEvent=kFALSE; // only release execute event lock when we finish the move JAM

   } // if(event==...)
}

const char* TGo4WinCondView::GetName() const
{
   if(fxWinCondition)
      return (fxWinCondition->GetName());
   else
      return 0;
}


void TGo4WinCondView::SetName(const char* nam)
{
   if(fxWinCondition) fxWinCondition->SetName(nam);
}

void TGo4WinCondView::SetToLimits(Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
if(fxWinCondition==0) return;
if(fxWinCondition->GetDimension()>1)
   fxWinCondition->SetValues(x1,x2,y1,y2);
else
   fxWinCondition->SetValues(x1,x2);
}


void TGo4WinCondView::SetLabelDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetLabelDraw(on);
}

void TGo4WinCondView::SetLimitsDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetLimitsDraw(on);
}

void TGo4WinCondView::SetIntDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetIntDraw(on);
}

void TGo4WinCondView::SetXMeanDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetXMeanDraw(on);
}

void TGo4WinCondView::SetYMeanDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetYMeanDraw(on);
}

void TGo4WinCondView::SetXRMSDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetXRMSDraw(on);
}

void TGo4WinCondView::SetYRMSDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetYRMSDraw(on);
}

void TGo4WinCondView::SetXMaxDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetXMaxDraw(on);
}

void TGo4WinCondView::SetYMaxDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetYMaxDraw(on);
}

void TGo4WinCondView::SetCMaxDraw(Bool_t on)
{
   if(fxWinCondition) fxWinCondition->SetCMaxDraw(on);
}


void TGo4WinCondView::SetNumFormat(const char* fmt)
{
   if(fxWinCondition) fxWinCondition->SetLabelNumFormat(fmt);
}


//void TGo4WinCondView::DeleteRegion()
//{
//   if(fxWinCondition && !fxWinCondition->IsOwnedByEditor())
//      {
//         fxWinCondition->Delete("");
//      }
//}

void TGo4WinCondView::SaveLabelStyle()
{
   if(fxWinCondition) fxWinCondition->SaveLabelStyle();
}

void TGo4WinCondView::ResetLabel()
{
   if(fxWinCondition) fxWinCondition->ResetLabel();
}

