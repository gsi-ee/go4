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

#include "TGo4ASImage.h"

#ifndef __NOGO4ASI__

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
   fxMaxY(1),
   fdWidth(0),
   fdHeight(0)
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

   Int_t numx = histo->GetNbinsX();
   Int_t numy = histo->GetNbinsY();

   TArrayD arr(numx*numy);
   for (int x=1; x<=numx; x++)
      for (int y=1; y<=numy; y++)
         arr[(y-1)*numx + (x-1)] = histo->GetBinContent(x, y);

   SetImage(arr, numx);
   SetName(histo->GetName());

   fdWidth = numx;
   fdHeight = numy;
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
   UInt_t width = UInt_t(fdWidth);
   UInt_t height = UInt_t(fdHeight);

   if ((rxmin<rxmax) && (fxMinX<fxMaxX)) {
      if (rxmin>=fxMinX)
        offX = UInt_t((rxmin-fxMinX)/(fxMaxX-fxMinX)*fdWidth);
      if (rxmax<=fxMaxX)
        width = UInt_t((rxmax-fxMinX)/(fxMaxX-fxMinX)*fdWidth) - offX;
   }

   if ((rymin<rymax) && (fxMinY<fxMaxY)) {
      if (rymin>=fxMinY)
        offY = UInt_t((rymin-fxMinY)/(fxMaxY-fxMinY)*fdHeight);
      if (rymax<=fxMaxY)
        height = UInt_t((rymax-fxMinY)/(fxMaxY-fxMinY)*fdHeight) - offY;
   }

   TASImage::Zoom(offX, offY, width, height);
}

void TGo4ASImage::Zoom(UInt_t offX, UInt_t offY, UInt_t width, UInt_t height)
{
   TASImage::Zoom(offX, offY, width, height);

   double rxmin = fZoomOffX/fdWidth*(fxMaxX-fxMinX)+fxMinX;
   double rxmax = (fZoomOffX+fZoomWidth)/fdWidth*(fxMaxX-fxMinX)+fxMinX;
   double rymin = fZoomOffY/fdHeight*(fxMaxY-fxMinY)+fxMinY;
   double rymax = (fZoomOffY+fZoomHeight)/fdHeight*(fxMaxY-fxMinY)+fxMinY;

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
