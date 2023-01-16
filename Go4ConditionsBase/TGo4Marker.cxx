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

#include "TGo4Marker.h"

#include "TH1.h"
#include "TROOT.h"
#include "TVirtualPad.h"

#include "TGo4MarkerPainter.h"
#include "TGo4Log.h"
#include "TGo4Status.h"

UInt_t TGo4Marker::fguInstanceCounter = 0;

Bool_t TGo4Marker::fgbHASLABEL = kTRUE;
Bool_t TGo4Marker::fgbHASCONNECTOR = kTRUE;
Bool_t TGo4Marker::fgbXDRAW = kTRUE;
Bool_t TGo4Marker::fgbYDRAW = kTRUE;
Bool_t TGo4Marker::fgbXBINDRAW = kFALSE;
Bool_t TGo4Marker::fgbYBINDRAW = kFALSE;
Bool_t TGo4Marker::fgbCONTDRAW = kTRUE;
TString TGo4Marker::fgxNUMFORMAT = "%.4E";


Int_t TGo4Marker::GetInstances()
{
   return fguInstanceCounter;
}


TGo4Marker::TGo4Marker(Double_t x, Double_t y, Int_t style) : TMarker(x,y,style)
{
   fxName = "Marker ";
   fxName += ++fguInstanceCounter;
   InitLabelStyle();
   SetBit(kMustCleanup);
}

TGo4Marker::TGo4Marker() : TMarker()
{
//++fguInstanceCounter; // do not increment instances by streaming
   InitLabelStyle();
   SetBit(kMustCleanup);
}

TGo4Marker::~TGo4Marker()
{
   UnDraw();
   if(fxPainter) delete fxPainter;
//   if(fguInstanceCounter>0) --fguInstanceCounter;
}

void TGo4Marker::SetDrawPad(TVirtualPad* pad)
{
   fxDrawPad=pad;
}

Bool_t TGo4Marker::CheckDrawPad() const
{
   return fxDrawPad != nullptr;
}

void TGo4Marker::SetHistogram(TH1* histo)
{
   fxHisto = histo;
}

Bool_t TGo4Marker::CheckHistogram() const
{
   return fxHisto != nullptr;
}


void TGo4Marker::Paint(Option_t* opt)
{
   //if(!IsPainted() || GetDrawPad()!=gPad) return;
   /////// check for streamed canvas markers that were not Draw()n:
   if(!CheckDrawPad() && !IsPainted())
      {
        SetDrawPad(gPad);
        if(IsVisible()) SetPainted(kTRUE);
      }
   if(!IsPainted() || GetDrawPad()!=gPad) return; // supress invisible markers (optional)
   TMarker::Paint(opt);
   if(!fxPainter) fxPainter = CreatePainter();
   // marker subclass may not provide a real painter, then we skip painting:
   if(fxPainter) fxPainter->PaintLabel(opt);
}

void TGo4Marker::Draw(Option_t* opt)
{
   if(IsVisible()) {
      if(gPad && gPad->GetListOfPrimitives()->FindObject(this)==nullptr) {
         UnDraw();
         AppendPad(opt);
         SetPainted(kTRUE);
      }
   } else
      UnDraw(opt);

   SetDrawPad(gPad); // need this to calculate coords even for invisible marker
}

void TGo4Marker::UnDraw(Option_t* opt)
{
   SetPainted(kFALSE);
   SetDrawPad(nullptr);
   gROOT->GetListOfCanvases()->RecursiveRemove(this);

   // marker subclass may not provide a real painter, then we skip unpainting:
   if(fxPainter) fxPainter->UnPaintLabel();
}

void TGo4Marker::Pop()
{
   if(fxPainter) fxPainter->DisplayToFront();
}


void TGo4Marker::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   if (!gPad)
      return;
   TMarker::ExecuteEvent(event, px, py);
   if (event == kButton1Up) {
      if (fxPainter)
         fxPainter->DisplayToFront();
   }
}

TGo4MarkerPainter* TGo4Marker::CreatePainter()
{
   TGo4MarkerPainter* painter = new TGo4MarkerPainter(GetName());
   painter->SetMarker(this);
   return painter;
}

void TGo4Marker::SetToBin(Int_t xbin, Int_t ybin)
{
   if (!CheckHistogram())
      return;
   Int_t bin = fxHisto->GetBin(xbin, ybin);
   Stat_t content = fxHisto->GetBinContent(bin);
   Double_t x = fxHisto->GetXaxis()->GetBinCenter(xbin);
   Double_t y = 0;
   if (fxHisto->GetDimension() == 1) {
      y = content;
   } else if (fxHisto->GetDimension() == 2) {
      y = fxHisto->GetYaxis()->GetBinCenter(ybin);
   }
   SetX(x);
   SetY(y);
}

Int_t TGo4Marker::GetXbin() const
{
   if (!CheckHistogram() || !CheckDrawPad())
      return -1;
   return fxHisto->GetXaxis()->FindFixBin(fxDrawPad->PadtoX(GetX()));
}

Int_t TGo4Marker::GetYbin() const
{
   if (!CheckHistogram() || !CheckDrawPad())
      return -1;
   return fxHisto->GetYaxis()->FindFixBin(fxDrawPad->PadtoY(GetY()));
}

Int_t TGo4Marker::GetCont() const
{
   if (!CheckHistogram() || !CheckDrawPad())
      return -1;
   Int_t bin = fxHisto->GetBin(GetXbin(), GetYbin());
   return (Int_t) fxHisto->GetBinContent(bin);
}

Double_t TGo4Marker::GetLabelX() const
{
   if (!CheckDrawPad())
      return 0;
   Double_t xoff = 0.01 * (fxDrawPad->GetUxmax() - fxDrawPad->GetUxmin());
   Double_t xpos = GetX() + xoff;
   return xpos;
}

Double_t TGo4Marker::GetLabelY() const
{
   if (!CheckDrawPad())
      return 0;
   Double_t yoff = 0.01 * (fxDrawPad->GetUymax() - fxDrawPad->GetUymin());
   Double_t ypos = 0;
   if (GetY() == 0)
      ypos = (fxDrawPad->GetUymax() - fxDrawPad->GetUymin()) / 2; // case of 1d histo
   else
      ypos = GetY() + yoff; // 2d histo near cursor point
   return ypos;
}

void TGo4Marker::Print(Option_t *opt) const
{
   TString textbuffer = "Marker ";
   textbuffer += GetName();
   textbuffer += " with";
   TH1 *hist = GetHistogram();
   if (hist) {
      textbuffer += " histogram: ";
      textbuffer += hist->GetName();
   } else
      textbuffer += "out histogram";

   textbuffer += TString::Format("\n    X:  \t\tY:  \t\tXbin:\t\tYbin:\t\tCounts:\n    %.2f\t\t%.2f\t\t%d\t\t%d\t\t%d",
                                 GetX(), GetY(), GetXbin(), GetYbin(), GetCont());

   TString option = opt;
   option.ToLower();
   if (option.Contains("go4log"))
      TGo4Log::Message(1, "%s", textbuffer.Data());
   else
      TGo4Status::PrintLine("%s", textbuffer.Data());
}

void TGo4Marker::SaveLabelStyle()
{
   TGo4Marker::fgbHASLABEL = fbHasLabel;
   TGo4Marker::fgbHASCONNECTOR = fbHasConnector;
   TGo4Marker::fgbXDRAW = fbXDraw;
   TGo4Marker::fgbYDRAW = fbYDraw;
   TGo4Marker::fgbXBINDRAW = fbXbinDraw;
   TGo4Marker::fgbYBINDRAW = fbYbinDraw;
   TGo4Marker::fgbCONTDRAW = fbContDraw;
   TGo4Marker::fgxNUMFORMAT = fxNumFormat;
}

void TGo4Marker::InitLabelStyle()
{
   fbHasLabel = TGo4Marker::fgbHASLABEL;
   fbHasConnector = TGo4Marker::fgbHASCONNECTOR;
   fbXDraw = TGo4Marker::fgbXDRAW;
   fbYDraw = TGo4Marker::fgbYDRAW;
   fbXbinDraw = TGo4Marker::fgbXBINDRAW;
   fbYbinDraw = TGo4Marker::fgbYBINDRAW;
   fbContDraw = TGo4Marker::fgbCONTDRAW;
   fxNumFormat = TGo4Marker::fgxNUMFORMAT;
}

void TGo4Marker::SetGlobalStyle(Bool_t HASLABEL, Bool_t HASCONNECTOR, Bool_t XDRAW, Bool_t YDRAW,
                                Bool_t XBINDRAW, Bool_t YBINDRAW, Bool_t CONTDRAW, const char *NUMFORMAT)
{
   TGo4Marker::fgbHASLABEL = HASLABEL;
   TGo4Marker::fgbHASCONNECTOR = HASCONNECTOR;
   TGo4Marker::fgbXDRAW = XDRAW;
   TGo4Marker::fgbYDRAW = YDRAW;
   TGo4Marker::fgbXBINDRAW = XBINDRAW;
   TGo4Marker::fgbYBINDRAW = YBINDRAW;
   TGo4Marker::fgbCONTDRAW = CONTDRAW;
   TGo4Marker::fgxNUMFORMAT = NUMFORMAT;

}

void TGo4Marker::GetGlobalStyle(Bool_t &HASLABEL, Bool_t &HASCONNECTOR, Bool_t &XDRAW, Bool_t &YDRAW,
                                Bool_t &XBINDRAW, Bool_t &YBINDRAW, Bool_t &CONTDRAW, TString &NUMFORMAT)
{
   HASLABEL = TGo4Marker::fgbHASLABEL;
   HASCONNECTOR = TGo4Marker::fgbHASCONNECTOR;
   XDRAW = TGo4Marker::fgbXDRAW;
   YDRAW = TGo4Marker::fgbYDRAW;
   XBINDRAW = TGo4Marker::fgbXBINDRAW;
   YBINDRAW = TGo4Marker::fgbYBINDRAW;
   CONTDRAW = TGo4Marker::fgbCONTDRAW;
   NUMFORMAT = TGo4Marker::fgxNUMFORMAT;
}


void TGo4Marker::ResetLabel()
{
   if (fxPainter) {
      fxPainter->UnPaintLabel("reset");
      fxPainter->PaintLabel();
   }
}

void TGo4Marker::DeletePainter()
{
   if (fxPainter) {
      delete fxPainter;
      fxPainter = nullptr;
   }
}
