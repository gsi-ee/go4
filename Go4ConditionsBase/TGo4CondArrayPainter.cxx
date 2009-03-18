#include "TGo4CondArrayPainter.h"

#include "TVirtualPad.h"

#include "TGo4WinCond.h"
#include "TGo4WinCondPainter.h"
#include "TGo4PolyCondPainter.h"
#include "TGo4CondArray.h"

TGo4CondArrayPainter::TGo4CondArrayPainter()
: TGo4ConditionPainter()
{
}

TGo4CondArrayPainter::TGo4CondArrayPainter(const Text_t* name)
: TGo4ConditionPainter(name,"TGo4CondArrayPainter")
{
}

TGo4CondArrayPainter::~TGo4CondArrayPainter()
{
   UnPaintCondition();
}

void TGo4CondArrayPainter::PaintCondition(Option_t* opt)
{
if(gPad==0) return;
TGo4CondArray* arconny=dynamic_cast<TGo4CondArray*>(fxCondition);
if(arconny)
   {
   TH1* workhis=arconny->GetWorkHistogram();
   Int_t number=arconny->GetNumber();
   Int_t selected=arconny->GetCurrentIndex();
   for(Int_t i=0; i<number; ++i)
      {
         TGo4Condition* con=arconny->At(i);
         if(con)
            {
              con->SetOwnedByEditor(kTRUE); // prevent interactive deletion
              con->SetWorkHistogram(workhis);
              con->SetLineColor((i)%6 +2);
              con->SetFillColor((i)%6 +2);
              if(dynamic_cast<TGo4WinCond*>(con))
                 {
                    if(!arconny->IsMultiEdit() && i==selected)
                        {
                           con->SetFillStyle(3002);
                        }
                    else
                        {
                           con->SetFillStyle(0);
                        }
                 }
              else
                 {
                     if(!arconny->IsMultiEdit() && i==selected)
                        {
                           con->SetLineStyle(1);
                        }
                    else
                        {
                           con->SetLineStyle(4);
                        }
                 }
              con->Paint(opt);
            }
      }
   } // if(arconny)
}

void TGo4CondArrayPainter::UnPaintCondition(Option_t* opt)
{
if(gPad==0) return;
TGo4CondArray* arconny=dynamic_cast<TGo4CondArray*>(fxCondition);
if(arconny)
   {
   Int_t number=arconny->GetNumber();
   for(Int_t i=0; i<number; ++i)
      {
         TGo4Condition* con=arconny->At(i);
         if(con) con->UnDraw(opt);
      }
   }
}

void TGo4CondArrayPainter::PaintLabel(Option_t* )
{
 // do not create a label for the complete array
}
