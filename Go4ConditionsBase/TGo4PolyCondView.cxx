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

#include "TGo4PolyCondView.h"

#include "TCutG.h"
#include "TPad.h"
#include "Buttons.h"

#include "TGo4PolyCond.h"

/** JAM2016: like for polygon condition we may supress intermediate streaming when updating
 * the condition from view by disabling the following define:*/
//#define POLYCONDVIEW_UPDATE_WITHCLONE 1


TGo4PolyCondView::TGo4PolyCondView(TCutG* source) :
   TCutG(), fbExecutesMouseEvent(kFALSE),fbExecutesMouseMenu(kFALSE), fxPolyCondition(0)
{
   SetCut(source);
   SetBit(kMustCleanup);
   //SetBit(kCanDelete, kFALSE);
   //std::cout<< "TGo4PolyCondView "<< (long) this <<" ctor from cut"<< (long) source<< std::endl;
   TGo4PolyCond::CleanupSpecials(); // JAM2016 - immediately take us out of special list
}

TGo4PolyCondView::TGo4PolyCondView() :
   TCutG(), fbExecutesMouseEvent(kFALSE),fbExecutesMouseMenu(kFALSE),fxPolyCondition(0)
{
   SetBit(kMustCleanup);
   //SetBit(kCanDelete, kFALSE);
  //std::cout<< "TGo4PolyCondView"<< (long) this <<" default ctor"<< std::endl;
   TGo4PolyCond::CleanupSpecials(); // JAM2016 - immediately take us out of special list
}


TGo4PolyCondView::~TGo4PolyCondView()
{
 // std::cout<< "TGo4PolyCondView "<< (long) this <<" dtor"<< std::endl;
}

void TGo4PolyCondView::Paint(Option_t* opt)
{

    //std::cout<<"TGo4PolyCondView::Paint, this="<< (long) this <<", Threadid="<< (long) pthread_self()<<", polycondition:"<< (long) fxPolyCondition<< std::endl;
    if(fxPolyCondition) TCutG::Paint(opt);
   // we have a valid condition set from painter.Otherwise,
   // suppress painting of condition view without owner. This case
   // happens after inserting canvas in go4 (separate cloning
   // of lists of primitive members), thus leading to a
   // second "ghost condition polygon"
}

Int_t TGo4PolyCondView::InsertPoint()
{
   fbExecutesMouseMenu=kTRUE;
   //std::cout<<"TGo4PolyCondView::InsertPoint, this="<< (long) this <<", Threadid="<< (long) pthread_self()<< std::endl;
   Int_t rev = TGraph::InsertPoint();
   UpdateCondition();
   fbExecutesMouseMenu=kFALSE;
   return rev;
}

Int_t TGo4PolyCondView::RemovePoint()
{
   fbExecutesMouseMenu=kTRUE;
   Int_t rev = TGraph::RemovePoint();
   UpdateCondition();
   fbExecutesMouseMenu=kFALSE;
   return rev;
}

void TGo4PolyCondView::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

 // std::cout<<"TGo4PolyCondView::ExecuteEvent, Threadid="<< (long) pthread_self()<< std::endl;
// TODO: do we really need this for newer ROOT versions?
#if 1
////// begin button double workaround JA
// the following is necessary to avoid bug in ROOT TGraph::ExecuteEvent
// that does not handle the kButton1Up after kButton1Double
// (temporary coordinates array not allocated for button double,
// thus crash in button up)
// we supress the button up event after button double for the moment...
static Bool_t ignoreNext=kFALSE;
if(ignoreNext)
   {
      //std::cout <<"PolyCondView supressing event "<<event << std::endl;
      ignoreNext=kFALSE;
      return;
   }
if(event==kButton1Double)
   {
      //std::cout <<"PolyCondView supressing double click" << std::endl;
      ignoreNext=kTRUE;
      fbExecutesMouseEvent=kTRUE;
      return;
   }
/////// end button double workaround JA
#endif

// JAM2016: try same mouse event locking as for window condition.
if(event==kButton1Down && fxPolyCondition)
{
  //std::cout <<"PolyCondView ExecuteEvent for button 1 down" << std::endl;
  fbExecutesMouseEvent=kTRUE; // only lock painting if we really touch the polygon JAM
}


//TGo4PolyCond::CleanupSpecials(); // JAM2016
TCutG::ExecuteEvent(event,px,py);
//TGo4PolyCond::CleanupSpecials(); // JAM 2016 avoid some intermediate TCutG objects in global list? NO!
if(event==kButton1Up && fxPolyCondition)
   {
      //std::cout <<"PolyCondView ExecuteEvent for button 1 up" << std::endl;
      Pop(); // do condition under edit into foreground, for condarray
      if(!fbExecutesMouseMenu && IsCutChanged())
         {
            UpdateCondition();
         }
      fxPolyCondition->ResetLabel("pop"); // always pop our label to foreground

      fbExecutesMouseEvent=kFALSE; //end update lock
  }// if(event==...)
}

Bool_t TGo4PolyCondView::IsCutChanged()
{
if(fxPolyCondition==0) return kFALSE;
Bool_t needsupdate=kFALSE;
Double_t epsilon=fxPolyCondition->GetUpdateEpsilon();
TCutG* concut=fxPolyCondition->GetCut(kFALSE);
// check if update is needed:
if(concut)
   {
      Int_t oldmaxpoints=concut->GetN();
      Int_t newmaxpoints=GetN();
      if(oldmaxpoints!=newmaxpoints)
         {
             needsupdate=kTRUE;
         }
      else
         {
            for(Int_t point=0; point<oldmaxpoints; ++point)
               {
                 Double_t xn=0;
                 Double_t yn=0;
                 Double_t xo=0;
                 Double_t yo=0;
                 concut->GetPoint(point,xo,yo);
                 GetPoint(point,xn,yn);
                 if(TMath::Abs(xo-xn) > epsilon)
                     {
                        needsupdate=kTRUE;
                        break;
                     }
                 if(TMath::Abs(yo-yn) > epsilon)
                     {
                        needsupdate=kTRUE;
                        break;
                     }
               }//for
         } // if(oldmaxpoints!=...)
   }// if(concut)
return needsupdate;
}

void TGo4PolyCondView::UpdateCondition()
{
   if(fxPolyCondition==0) return;
   //std::cout<< "TGo4PolyCondView "<< (long) this <<" ::UpdateCondition"<< std::endl;

#ifdef POLYCONDVIEW_UPDATE_WITHCLONE

   TCutG* ccut = CreateCut();
   fxPolyCondition->SetValues(ccut);
   delete ccut;
   //std::cout<< "TGo4PolyCondView::UpdateCondition has deleted intermediat TCutG "<< (long) ccut<< std::endl;
//   fxPolyCondition->SetValuesDirect(ccut); // change ownership of ccut
   TGo4PolyCond::CleanupSpecials();
#else
   fxPolyCondition->SetValues(this); // probably this is problematic if update with cloning is enabled in condition class!
#endif
   fxPolyCondition->SetChanged(kTRUE);
}


void TGo4PolyCondView::SetCut(TCutG* source)
{
  // std::cout<< "TGo4PolyCondView "<< (long) this <<" ::SetCut from cut"<< (long) source<< std::endl;

   if(source==0)
   {
     Set(0); // clear array of points
     SetPoint(0,0,0); //dummy to suppress empty graph warnings
   }
   else {
      Int_t pn=source->GetN();
      Set(pn); // set to new number of points
      Double_t xp=0;
      Double_t yp=0;
      for(Int_t i=0; i<pn; ++i) {
         source->GetPoint(i,xp,yp);
         SetPoint(i,xp,yp);
      }
   }
}


TCutG* TGo4PolyCondView::CreateCut()
{
   TCutG* result = new TCutG;
   result->SetBit(kMustCleanup);
   //result->SetBit(kCanDelete,kFALSE);
   TGo4PolyCond::CleanupSpecials(); // JAM2016 - immediately take us out of special list
   Int_t pn = GetN();
   Double_t xp=0;
   Double_t yp=0;
   for(Int_t i=0; i<pn; ++i) {
      GetPoint(i,xp,yp);
      result->SetPoint(i,xp,yp);
   }
   result->SetLineColor(GetLineColor());
   result->SetLineWidth(GetLineWidth());
   result->SetLineStyle(GetLineStyle());
   result->SetFillColor(GetFillColor());
   result->SetFillStyle(GetFillStyle());
   return result;
}

const char* TGo4PolyCondView::GetName() const
{
   if(fxPolyCondition)
      return (fxPolyCondition->GetName());
   else
      return (TCutG::GetName()); // JAM2016
}

void TGo4PolyCondView::SetName(const char* nam)
{
   if(fxPolyCondition) fxPolyCondition->SetName(nam);
   TCutG::SetName(nam); // JAM2016
}

void TGo4PolyCondView::SetLabelDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetLabelDraw(on);
}
void TGo4PolyCondView::SetLimitsDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetLimitsDraw(on);
}

void TGo4PolyCondView::SetIntDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetIntDraw(on);
}

void TGo4PolyCondView::SetXMeanDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetXMeanDraw(on);
}

void TGo4PolyCondView::SetYMeanDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetYMeanDraw(on);
}

void TGo4PolyCondView::SetXRMSDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetXRMSDraw(on);
}

void TGo4PolyCondView::SetYRMSDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetYRMSDraw(on);
}

void TGo4PolyCondView::SetXMaxDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetXMaxDraw(on);
}

void TGo4PolyCondView::SetYMaxDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetYMaxDraw(on);
}

void TGo4PolyCondView::SetCMaxDraw(Bool_t on)
{
   if(fxPolyCondition) fxPolyCondition->SetCMaxDraw(on);
}

void TGo4PolyCondView::DeleteRegion()
{
   if(fxPolyCondition && !fxPolyCondition->IsOwnedByEditor()) fxPolyCondition->Delete("");
}

void TGo4PolyCondView::SaveLabelStyle()
{
   if(fxPolyCondition) fxPolyCondition->SaveLabelStyle();
}

void TGo4PolyCondView::ResetLabel()
{
   if(fxPolyCondition) fxPolyCondition->ResetLabel();
}
