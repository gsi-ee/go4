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

#ifndef TGO4ASIMAGE_H
#define TGO4ASIMAGE_H

class TH2;
class TPad;
class TGo4ViewPanel;

#ifndef __NOGO4ASI__

#include "TASImage.h"

  /** Special ASImage for display of 2d histograms in go4 viewpanel.
    * has backpointer to viewpanel and reacts explicitely when user select
    * new range in image
    * @author J. Adamczewski
    * @author S. Linev
    * @since 25-Mar-2004
    */
class TGo4ASImage : public TASImage {
   public:
      TGo4ASImage();
      virtual ~TGo4ASImage();

      void SetHistogramContent(TH2* histo);

      void SetDrawData(TH2* histo, TGo4ViewPanel* panel, TPad* pad);

      void SetSelectedRange(double rxmin, double rxmax, double rymin, double rymax);

      virtual void Zoom(UInt_t offX, UInt_t offY, UInt_t width, UInt_t height); //*MENU*

      virtual void UnZoom(); //*MENU*

      /** Use this to close palette editor properly */
      void ClosePaletteEditor();

      // old code
      TGo4ASImage(TH2*) {}

      TH2* GetHistogram() const { return 0; }

      /** sets limits of histo according to current zoom scale of image */
      void SetHistogramRanges(TH2*) {}

      /** sets zoom of image according to current limits of histo */
      void SetZoom(TH2*) {}

   private:

      TGo4ViewPanel* fxPanel; //!

      TPad*         fxPad;   //!

      double        fxMinX;  //!
      double        fxMaxX;  //!
      double        fxMinY;  //!
      double        fxMaxY;  //!

      double        fdWidth;  //!  original width of image (histogram)
      double        fdHeight; //!  original height of image (histogram)


};

#else

// give a dummy for the Qt Slots!
#include "TObject.h"

class TGo4ASImage : public TObject {
   public:
      TGo4ASImage() {}
      virtual ~TGo4ASImage() {}

      void SetHistogramContent(TH2*) {}

      void SetDrawData(TH2*, TGo4ViewPanel*, TPad*) {}

      void SetSelectedRange(double, double, double, double) {}
};

#endif // ASImage disable switch

#endif //TGO4ASIMAGE_H
