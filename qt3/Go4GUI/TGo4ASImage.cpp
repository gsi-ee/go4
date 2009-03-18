#include "TGo4ASImage.h"

#ifdef __GO4ASI__

#include "TH2.h"
#include "TGo4Log.h"
#include "TGo4ViewPanel.h"

TGo4ASImage::TGo4ASImage() :
   TASImage(),
   fxPanel(0),
   fxPad(0),
   fxMinX(0),
   fxMaxX(1),
   fxMinY(0),
   fxMaxY(1)
{
   SetConstRatio(kFALSE);
}

TGo4ASImage::~TGo4ASImage()
{
   ClosePaletteEditor();
}

void TGo4ASImage::SetHistogramContent(TH2* histo)
{
   if (histo==0) return;

   Int_t arsize = (histo->GetNbinsX()+2) * (histo->GetNbinsY()+2);
   Int_t width = histo->GetNbinsX()+2;
   TArrayD arr(arsize);
   for(int n=0;n<arsize;n++)
      arr[n] = histo->GetBinContent(n);

   SetImage(arr,width);
   SetName(histo->GetName());
}

void TGo4ASImage::SetDrawData(TH2* histo, TGo4ViewPanel* panel, TPad* pad)
{
   fxPanel = panel;
   fxPad = pad;

   if (histo==0) return;

   fxMinX = histo->GetXaxis()->GetXmin();
   fxMaxX = histo->GetXaxis()->GetXmax();
   fxMinY = histo->GetYaxis()->GetXmin();
   fxMaxY = histo->GetYaxis()->GetXmax();
}

void TGo4ASImage::SetSelectedRange(double rxmin, double rxmax, double rymin, double rymax)
{
   if ((rxmin>=rxmax) && (rymin>=rymax)) {
      TASImage::UnZoom();
      return;
   }

   UInt_t offX = 0;
   UInt_t offY = 0;
   UInt_t width = GetWidth();
   UInt_t height = GetHeight();

   if ((rxmin<rxmax) && (fxMinX<fxMaxX)) {
      if (rxmin>=fxMinX)
        offX = UInt_t((rxmin-fxMinX)/(fxMaxX-fxMinX)*GetWidth());
      if (rxmax<=fxMaxX)
        width = UInt_t((rxmax-fxMinX)/(fxMaxX-fxMinX)*GetWidth()) - offX;
   }

   if ((rymin<rymax) && (fxMinY<fxMaxY)) {
      if (rymin>=fxMinY)
        offY = UInt_t((rymin-fxMinY)/(fxMaxY-fxMinY)*GetHeight());
      if (rymax<=fxMaxY)
        height = UInt_t((rymax-fxMinY)/(fxMaxY-fxMinY)*GetHeight()) - offY;
   }

   TASImage::Zoom(offX, offY, width, height);
}


void TGo4ASImage::Zoom(UInt_t offX, UInt_t offY, UInt_t width, UInt_t height)
{
   TASImage::Zoom(offX, offY, width, height);

   double rxmin = 1.*offX/GetWidth()*(fxMaxX-fxMinX)+fxMinX;
   double rxmax = 1.*(offX+width)/GetWidth()*(fxMaxX-fxMinX)+fxMinX;
   double rymin = 1.*offY/GetHeight()*(fxMaxY-fxMinY)+fxMinY;
   double rymax = 1.*(offY+height)/GetHeight()*(fxMaxY-fxMinY)+fxMinY;
   if (fxPanel!=0)
      fxPanel->PadRangeAxisChanged(fxPad, rxmin, rxmax, rymin, rymax);
}

void TGo4ASImage::UnZoom()
{
   TASImage::UnZoom();
   if (fxPanel!=0)
      fxPanel->PadRangeAxisChanged(fxPad,0.,0.,0.,0.);
}

void TGo4ASImage::ClosePaletteEditor()
{
   if(fPaletteEditor!=0) {
      fPaletteEditor->CloseWindow();
      fPaletteEditor=0;
   }
}

#endif // asimage disable switch
