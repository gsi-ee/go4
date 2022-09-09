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

#include "TGo4FitGuiArrow.h"

#include "Buttons.h"

#include "TGo4FitPanel.h"
#include "TGo4FitModel.h"
#include "TGo4ViewPanel.h"

TGo4FitGuiArrow::TGo4FitGuiArrow() :
    TArrow(), fxType(at_none), fxModel(nullptr), fxOther(nullptr), fdLineAmpl(0.), fxComp(nullptr), fiRangeNum(0),
    fdRangeY(0.), fdShiftX(0.), fxItem(nullptr), fxPanel(nullptr)
{
}

TGo4FitGuiArrow::TGo4FitGuiArrow(Float_t arrowsize, Option_t *option) :
    TArrow(0.,0.,0.,0.,arrowsize,option), fxType(at_none), fxModel(nullptr), fxOther(nullptr), fdLineAmpl(0.), fxComp(nullptr), fiRangeNum(0),
    fdRangeY(0.), fdShiftX(0.), fxItem(nullptr), fxPanel(nullptr)
{
}

TGo4FitGuiArrow::TGo4FitGuiArrow(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Float_t arrowsize, Option_t* option) :
    TArrow(x1,y1,x2,y2,arrowsize,option), fxType(at_none), fxModel(nullptr), fxOther(nullptr), fdLineAmpl(0.), fxComp(nullptr), fiRangeNum(0),
    fdRangeY(0.), fdShiftX(0.), fxItem(nullptr), fxPanel(nullptr)
{
}

TGo4FitGuiArrow::~TGo4FitGuiArrow()
{
}

void TGo4FitGuiArrow::SetForPosition(TGo4FitModel* model, TGo4FitGuiArrow* forwidth, Double_t iLineAmpl)
{
  fxType = at_pos;
  fxModel = model;
  fxOther = forwidth;
  fdLineAmpl = iLineAmpl;
}

void TGo4FitGuiArrow::SetForWidth(TGo4FitModel* model, TGo4FitGuiArrow* forposition)
{
  fxType = at_width;
  fxModel = model;
  fxOther = forposition;
}

void TGo4FitGuiArrow::SetForRange(TGo4FitComponent* Comp, Int_t num, Double_t rangey, Double_t shiftx)
{
  fxType = at_range;
  fxComp = Comp;
  fiRangeNum = num;
  fdRangeY = rangey;
  fdShiftX = shiftx;
}

void TGo4FitGuiArrow::SetItem(QFitItem* item, TGo4FitPanel* panel)
{
   fxItem = item;
   fxPanel = panel;
}

Bool_t TGo4FitGuiArrow::Locate()
{
   Double_t x1 = GetX1(), x2 = GetX2(), y1 = GetY1(), y2 = GetY2();

   switch (fxType) {
      case at_pos:
         Double_t pos;
         if (fxModel->GetPosition(0,pos)) {
            SetX1(pos); SetX2(pos); SetY1(fdLineAmpl/2); SetY2(fdLineAmpl);
         }
         break;
      case at_width:
         Double_t width, ppp;
         if ((fxModel->GetWidth(0,width)) && (fxModel->GetPosition(0,ppp))) {
            SetX1(ppp-width); SetX2(ppp+width);
            SetY1(fxOther->GetY1());
            SetY2(fxOther->GetY1());
         }
         break;
      case at_range:
         Int_t typ, naxis;
         Double_t left, right;
         fxComp->GetRangeCondition(fiRangeNum, typ, naxis, left, right);
         switch (typ) {
            case 0: SetX1(left); SetX2(right); break;
            case 1: SetX1(left); SetX2(right); break;
            case 2: SetX1(left); SetX2(left+fdShiftX); break;
            case 3: SetX1(right-fdShiftX); SetX2(right); break;
         }
         SetY1(fdRangeY); SetY2(fdRangeY);

         break;
   }

   return (x1 != GetX1()) || (x2 != GetX2()) || (y1 != GetY1()) || (y2 != GetY2());
}

Bool_t TGo4FitGuiArrow::IsAssignTo(TObject *obj)
{
   return obj && ((obj==fxModel) || (obj==fxComp));
}

void TGo4FitGuiArrow::Delete(Option_t* option)
{
   if ((fxType==at_pos) && fxPanel)
      fxPanel->DeleteModelWithPrimit(this);
}

void TGo4FitGuiArrow::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   TArrow::ExecuteEvent(event,px,py);

   if (event == kButton1Up)
      TestNewPosition();
}

void TGo4FitGuiArrow::TestNewPosition()
{
   switch (fxType) {

      case at_pos: {
         fxModel->SetPosition(0, GetX2());
         fdLineAmpl = (GetY1() > GetY2()) ? GetY1() : GetY2();
         if (fxPanel)
            fxPanel->ArrowChanged(this);
         break;
      }

      case at_width: {
         Double_t width = 0;
         fxModel->GetWidth(0, width);
         Double_t pos = fxOther->GetX2();

         Double_t w0 = (GetX2() - GetX1()) / 2;
         Double_t w1 = pos - GetX1();
         Double_t w2 = GetX2() - pos;

         if (TMath::Abs(w0 - width) > 1e-10) {
            Double_t w = width;
            if (TMath::Abs(w1 - width) > 1e-10)
               w = w1;
            else
               w = w2;
            if (w > 0) {
               width = w;
               fxModel->SetWidth(0, width);
               if (fxPanel)
                  fxPanel->ArrowChanged(this);
            }
         }
         break;
      }

      case at_range: {
         Int_t typ, naxis;
         Double_t left, right;
         fxComp->GetRangeCondition(fiRangeNum, typ, naxis, left, right);

         left = GetX1() < GetX2() ? GetX1() : GetX2();
         right = GetX1() > GetX2() ? GetX1() : GetX2();

         fxComp->SetRangeCondition(fiRangeNum, typ, naxis, left, right);

         if (fxPanel)
            fxPanel->ArrowChanged(this);

         break;
      }
   }
}

#ifdef __GO4WEB__
Bool_t TGo4FitGuiArrow::Notify()
{
   if (fxPanel && fxPanel->ActivePanel() && fxPanel->ActivePanel()->IsWebCanvas())
      TestNewPosition();

   return TObject::Notify();
}
#endif
