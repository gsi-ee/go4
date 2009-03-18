#include "TGo4Marker.h"

#include "Riostream.h"

#include "TH1.h"
#include "TROOT.h"
#include "TVirtualPad.h"
#include "snprintf.h"

#include "TGo4MarkerPainter.h"
#include "TGo4Log.h"

UInt_t TGo4Marker::fguInstanceCounter=0;

Bool_t TGo4Marker::fgbHASLABEL=kTRUE;
Bool_t TGo4Marker::fgbHASCONNECTOR=kTRUE;
Bool_t TGo4Marker::fgbXDRAW=kTRUE;
Bool_t TGo4Marker::fgbYDRAW=kTRUE;
Bool_t TGo4Marker::fgbXBINDRAW=kFALSE;
Bool_t TGo4Marker::fgbYBINDRAW=kFALSE;
Bool_t TGo4Marker::fgbCONTDRAW=kTRUE;


TGo4Marker::TGo4Marker(Double_t x, Double_t y, Int_t style)
: TMarker(x,y,style),fxPainter(0),
   fbIsPainted(kFALSE), fbVisible(kTRUE), fbHasLabel(kTRUE), fbHasConnector(kTRUE),
   fbXDraw(kTRUE), fbYDraw(kTRUE), fbXbinDraw(kTRUE), fbYbinDraw(kTRUE), fbContDraw(kTRUE),
   fxHisto(0),fxDrawPad(0)
{
   fxName="Marker ";
   fxName+=++fguInstanceCounter;
   InitLabelStyle();
   SetBit(kMustCleanup);
}

TGo4Marker::TGo4Marker()
: TMarker(), fxPainter(0),
   fbIsPainted(kFALSE), fbVisible(kTRUE), fbHasLabel(kTRUE), fbHasConnector(kTRUE),
   fbXDraw(kTRUE), fbYDraw(kTRUE), fbXbinDraw(kTRUE), fbYbinDraw(kTRUE), fbContDraw(kTRUE),
   fxHisto(0),fxDrawPad(0)
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

Bool_t TGo4Marker::CheckDrawPad()
{
   return fxDrawPad!=0;
}

void TGo4Marker::SetHistogram(TH1* histo)
{
   fxHisto=histo;
}

Bool_t TGo4Marker::CheckHistogram()
{
   return fxHisto!=0;
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
   if(fxPainter==0) fxPainter=CreatePainter();
   // marker subclass may not provide a real painter, then we skip painting:
   if(fxPainter!=0) fxPainter->PaintLabel(opt);
}

void TGo4Marker::Draw(Option_t* opt)
{
   if(IsVisible()) {
      if(gPad && gPad->GetListOfPrimitives()->FindObject(this)==0) {
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
   SetDrawPad(0);
   gROOT->GetListOfCanvases()->RecursiveRemove(this);
   if(fxPainter==0) fxPainter = CreatePainter();
   // marker subclass may not provide a real painter, then we skip unpainting:
   if(fxPainter!=0) fxPainter->UnPaintLabel();
}

void TGo4Marker::Pop()
{
if(fxPainter!=0) fxPainter->DisplayToFront();
}


void TGo4Marker::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
if(gPad==0) return;
TMarker::ExecuteEvent(event,px,py);
if(event==kButton1Up)
   {
      if(fxPainter!=0) fxPainter->DisplayToFront();
   }
}


TGo4MarkerPainter* TGo4Marker::CreatePainter()
{
   TGo4MarkerPainter* painter=new TGo4MarkerPainter(GetName());
   painter->SetMarker(this);
   return painter;
}

void TGo4Marker::SetToBin(Int_t xbin, Int_t ybin)
{
if(!CheckHistogram()) return;
Int_t bin = fxHisto->GetBin(xbin,ybin);
Stat_t content = fxHisto->GetBinContent(bin);
Double_t x=fxHisto->GetXaxis()->GetBinCenter(xbin);
Double_t y=0;
if(fxHisto->GetDimension()==1)
   {
      y=content;
   }
else if (fxHisto->GetDimension()==2)
   {
      y=fxHisto->GetYaxis()->GetBinCenter(ybin);
   }
SetX(x);
SetY(y);
}


Int_t TGo4Marker::GetXbin()
{
if(!CheckHistogram() || !CheckDrawPad()) return -1;
Int_t binx  = fxHisto->GetXaxis()->FindFixBin(fxDrawPad->PadtoX(GetX()));
return binx;
}

Int_t TGo4Marker::GetYbin()
{
if(!CheckHistogram() || !CheckDrawPad()) return -1;
Int_t biny  = fxHisto->GetYaxis()->FindFixBin(fxDrawPad->PadtoY(GetY()));
return biny;

}

Int_t TGo4Marker::GetCont()
{
if(!CheckHistogram() || !CheckDrawPad())return -1;
Int_t bin = fxHisto->GetBin(GetXbin(),GetYbin());
Stat_t content = fxHisto->GetBinContent(bin);
return (Int_t) content;
}

Double_t TGo4Marker::GetLabelX()
{
if(!CheckDrawPad()) return 0;
Double_t xoff=0.01*(fxDrawPad->GetUxmax()-fxDrawPad->GetUxmin());
Double_t xpos=GetX()+xoff;
return xpos;
}

Double_t TGo4Marker::GetLabelY()
{
if(!CheckDrawPad()) return 0;
Double_t yoff=0.01*(fxDrawPad->GetUymax()-fxDrawPad->GetUymin());
Double_t ypos=0;
if(GetY()==0)
   ypos=(fxDrawPad->GetUymax()-fxDrawPad->GetUymin())/2; // case of 1d histo
else
   ypos=GetY()+yoff; // 2d histo near cursor point
return ypos;
}

void TGo4Marker::Print(Option_t* opt) const{
   TGo4Marker* localthis=const_cast<TGo4Marker*>(this);
   localthis->PrintMarker(opt);
}

//void TGo4Marker::DeleteMarker()
//{
//   Delete("");
//}


void TGo4Marker::PrintMarker(Option_t* opt)
{
TString option=opt;
option.ToLower();
TString textbuffer="Marker ";
textbuffer+=GetName();
textbuffer+=" with";
TH1* hist=GetHistogram();
if(hist)
   {
      textbuffer+=" histogram: ";
      textbuffer+=hist->GetName();
   }
else
   {
      textbuffer+="out histogram";
   }
Text_t buffer[256];
snprintf(buffer,256,
      "\n    X:  \t\tY:  \t\tXbin:\t\tYbin:\t\tCounts:\n    %.2f\t\t%.2f\t\t%d\t\t%d\t\t%d",
      GetX(), GetY(), GetXbin(),GetYbin(),GetCont());
textbuffer+=buffer;
if(option.Contains("go4log"))
   {
      TGo4Log::Message(1,textbuffer.Data());
   }
else
   {
      cout << textbuffer.Data() << endl;
   }
}

void TGo4Marker::SaveLabelStyle()
{
   TGo4Marker::fgbHASLABEL=fbHasLabel;
   TGo4Marker::fgbHASCONNECTOR=fbHasConnector;
   TGo4Marker::fgbXDRAW=fbXDraw;
   TGo4Marker::fgbYDRAW=fbYDraw;
   TGo4Marker::fgbXBINDRAW=fbXbinDraw;
   TGo4Marker::fgbYBINDRAW=fbYbinDraw;
   TGo4Marker::fgbCONTDRAW=fbContDraw;
}

void TGo4Marker::InitLabelStyle()
{
   fbHasLabel=TGo4Marker::fgbHASLABEL;
   fbHasConnector=TGo4Marker::fgbHASCONNECTOR;
   fbXDraw=TGo4Marker::fgbXDRAW;
   fbYDraw=TGo4Marker::fgbYDRAW;
   fbXbinDraw=TGo4Marker::fgbXBINDRAW;
   fbYbinDraw=TGo4Marker::fgbYBINDRAW;
   fbContDraw=TGo4Marker::fgbCONTDRAW;
}

void TGo4Marker::ResetLabel()
{
   if(fxPainter!=0) {
      fxPainter->UnPaintLabel("reset");
      fxPainter->PaintLabel();
   }
}

void TGo4Marker::DeletePainter()
{
   if (fxPainter!=0) {
      delete fxPainter;
      fxPainter = 0;
   }
}
