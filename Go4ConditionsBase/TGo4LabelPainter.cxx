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

#include "TGo4LabelPainter.h"

#include "TROOT.h"
#include "TVirtualPad.h"
#include "Riostream.h"

TGo4LabelPainter::TGo4LabelPainter() :
   TNamed(),
   fxLabel(0),
   fdX0(0),
   fdY0(0),
   fbIsLabStreamed(kTRUE)
{
   //std::cout <<"TGo4LabelPainter "<< (long)this<<" default ctor "<< std::endl;
   InitAttributes();
}

TGo4LabelPainter::TGo4LabelPainter(const char* name, const char* title) :
   TNamed(name, title ? title : "Go4 LabelPainter"),
   fxLabel(0),
   fdX0(0),
   fdY0(0),
   fbIsLabStreamed(kFALSE)
{
   //std::cout <<"TGo4LabelPainter "<< (long)this<<" ctor from name "<<name << std::endl;
   InitAttributes();
}


void TGo4LabelPainter::InitAttributes()
{
   SetBit(kMustCleanup);
   fdWidth=0.25;
   fdHeight=0.22;
   SetCaption("Empty label");
   SetTextAlign(12); // left and central aligned
   //SetTextSize(0.03); // % of pad height
   SetTextSize(0); // this will let root scale fontsize depending on label frame
   SetTextFont(102); // proportional font for alignment of equations
   SetTextColor(GetLineColor());
}

TGo4LabelPainter::~TGo4LabelPainter()
{
  //std::cout <<"TGo4LabelPainter dtor of "<< (long)this <<" will delete label"<< fxLabel << std::endl;
   if (fxLabel!=0) {
      delete fxLabel;
      fxLabel = 0;
   }
}

void TGo4LabelPainter::PaintLabel(Option_t* opt)
{
   if(gPad==0) return;
    Double_t xrange=(gPad->GetUxmax()-gPad->GetUxmin());
   Double_t yrange=(gPad->GetUymax()-gPad->GetUymin());
   if(!CheckLabel()) {
      // label was deleted by us or by root:
      if(fdX0==0)
        fdX0=xrange/2; // default: place at x center
      if(fdY0==0)
        fdY0=yrange/2; // default: place at y center
      // JAM: these initial coordinates can be problematic if assigned histogram is not yet drawn completely...
      fxLabel=CreateCurrentLabel(fdX0,fdY0);
      fxLabel->AppendPad(opt); // only append to pad if not already there
   } else {
      // label is exisiting:
      if (gPad->GetListOfPrimitives()->FindObject(fxLabel)==0)
         fxLabel->AppendPad(opt); // label was cleared from pad, we redraw it

      // JAM: catch here the case that label box was drawn with unsuited coordinates:
      if(((fxLabel->GetX2()- fxLabel->GetX1())<0.05*xrange) || (fxLabel->GetY2()- fxLabel->GetY1())<0.05*yrange)
        {
          Double_t x0=xrange/2;
          Double_t y0=yrange/2;
          Double_t xmax=0;
          Double_t ymax=0;
          LabelCoords(x0,y0,xmax,ymax);
          fxLabel->SetX1(x0);
          fxLabel->SetX2(xmax);
          fxLabel->SetY1(y0);
          fxLabel->SetY2(ymax);
          //printf("TGo4LabelPainter::PaintLabel corrected label coords to: (%f %f) (%f %f)",x0,y0,xmax,ymax);
        }

      fxLabel->SetLineColor(GetLineColor());
      fxLabel->Clear(); // remove old text lines
   }
   fxLabel->AddText(fxCaption.Data());
   fxLabel->AddText(" "); // dummy for automatic position of next line
   fxLabel->AddLine(0,0,0,0);

   //std::cout <<"TGo4LabelPainter::PaintLabel of this="<< (long)this <<" with label "<<(long) fxLabel << std::endl;

}

void TGo4LabelPainter::UnPaintLabel(Option_t* opt)
{
   gROOT->GetListOfCanvases()->RecursiveRemove(fxLabel);

   // we do not delete label, but restore graphics properties though invisible
   TString option=opt;
   if(option.Contains("reset")) {
      // case of reset option: discard old label geometry if
      if(CheckLabel()) {
         delete fxLabel;
         fxLabel=0;
      }
   }
}

void TGo4LabelPainter::RePaintLabel(Option_t* opt)
{
if(fxLabel)
      fxLabel->Paint(opt);

}

void TGo4LabelPainter::DisplayToFront(Option_t* opt)
{
if(fxLabel)
   fxLabel->Pop();

}



TGo4Label* TGo4LabelPainter::CreateCurrentLabel(Double_t x, Double_t y)
{
   if(gPad==0) return 0;
   Double_t x0=x;
   Double_t y0=y; // buffer external variables, since LabelCoords will change them
   Double_t xmax=0;
   Double_t ymax=0;
   LabelCoords(x0,y0,xmax,ymax);
   TGo4Label* label=new TGo4Label(x0,y0,xmax,ymax);
   //printf("TGo4LabelPainter::CreateCurrentLabel has new label=%d \n",(long) label); std::cout << std::endl;
   label->SetOwner(this);
   TAttText::Copy(*label);
   TAttLine::Copy(*label);
   TAttFill::Copy(*label);
   return label;
}

Bool_t TGo4LabelPainter::CheckLabel()
{
   if(fbIsLabStreamed && fxLabel!=0) {
      // case of label was streamed from file: not yet in cleanup list!
      fxLabel->SetOwner(this);
      fbIsLabStreamed=kFALSE;
      //std::cout <<"CheckLabel with fbIsLabStreamed" << std::endl;
      return kTRUE;
   }

   if(TGo4Label::fxLastDeleted==(char*) fxLabel)
      {
         // our label was deleted by user mouse menu (or pad clear!)just before
         TGo4Label::fxLastDeleted=0;
         fxLabel=0; // reset reference, will re-create label on next paint
         //std::cout <<"CheckLabel with lastdeleted case" << std::endl;
         return kFALSE;
      }
   //std::cout <<"CheckLabel sees label "<<(long) fxLabel <<" and returns "<< (bool) (fxLabel!=0) << std::endl;
   return fxLabel!=0;
}


void TGo4LabelPainter::LabelCoords(Double_t &xmin, Double_t &ymin, Double_t &xmax, Double_t &ymax)
{
   Double_t xpmin =gPad->GetUxmin();
   Double_t xpmax =gPad->GetUxmax();
   Double_t ypmin =gPad->GetUymin();
   Double_t ypmax =gPad->GetUymax(); // this delivers pad coordinates
   Double_t dx=(xpmax-xpmin);
   Double_t dy=(ypmax-ypmin);
   Double_t width=fdWidth*dx;
   Double_t height=fdHeight*dy; // width is in pad coordinates
   xmax=gPad->XtoPad(xmin)+width;
   ymax=gPad->YtoPad(ymin)+height; // calculate label geometry in pad coordinates
   xmax=gPad->PadtoX(xmax);
   ymax=gPad->PadtoY(ymax); // back to axis coordinates for initial pavetext
}

Double_t TGo4LabelPainter::GetLabelXlo()
{
if(fxLabel)
      return fxLabel->GetX1();
else
   return 0;
}

Double_t TGo4LabelPainter::GetLabelYlo()
{
if(fxLabel)
      return fxLabel->GetY1();
else
   return 0;
}

Double_t TGo4LabelPainter::GetLabelXup()
{
if(fxLabel)
   return fxLabel->GetX2();
else
   return 0;
}

Double_t TGo4LabelPainter::GetLabelYup()
{
if(fxLabel)
      return fxLabel->GetY2();
else
   return 0;
}

///////////// treat painting of the text label: ///////////

char* TGo4Label::fxLastDeleted = 0;

void TGo4Label::Paint(Option_t* opt)
{
if(gPad==0) return;
if(fxOwner)
{
  TPaveText::Paint(opt);
}

// suppress painting of labels without owner. This case
// happens after insert canvas in go4 (separate cloning
// of lists of primitive members), thus leading to a
// second "ghost label" for markers and conditions
}

///////////// use this to pop the label to front whenever touched
void TGo4Label::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
if(gPad==0) return;
TPaveText::ExecuteEvent(event,px,py);
if(event==kButton1Up)
   {
      TGo4LabelPainter* painter=dynamic_cast<TGo4LabelPainter*>(fxOwner);
      if(painter)
         {
            painter->DisplayToFront();
            //std::cout <<"TGo4Label::ExecuteEvent with Display to front for "<<painter->GetName() << std::endl;
         }
   }
}

//////////////////////////////////////////////////////////

char* TGo4LabelConnector::fxLastDeleted = 0;

///////////// treat painting of the text label connector:
void TGo4LabelConnector::Paint(Option_t* opt)
{
if(gPad==0) return;
if(fxOwner) TLine::Paint(opt);
// suppress painting of label connectors without owner. This case
// happens after insert canvas in go4 (separate cloning
// of lists of primitive members), thus leading to a
// second "ghost label connector" for markers
}
/////////////////////////////////////////////////////////
