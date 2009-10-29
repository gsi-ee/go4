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

#ifndef TGO4MARKERPAINTER_H
#define TGO4MARKERPAINTER_H

#include "TGo4LabelPainter.h"

class TGo4Marker;

class TGo4MarkerPainter : public TGo4LabelPainter {
public:
   TGo4MarkerPainter();
   TGo4MarkerPainter(const char* name, const char* title="Go4 MarkerPainter");
   virtual ~TGo4MarkerPainter();

  /** Display coordinate label for this marker. */
  virtual void PaintLabel(Option_t* opt="");
  virtual void UnPaintLabel(Option_t* opt="");
  void PaintConnector(Option_t* opt="");
  void UnPaintConnector(Option_t* opt="");
  virtual void DisplayToFront(Option_t* opt="");


  virtual void SetMarker(TGo4Marker* mark) {fxMarker=mark;}

protected:

     /** Check if connector is still existing. Uses root list of cleanups. */
    Bool_t CheckConnector();

   /** Marker to be painted.*/
   TGo4Marker* fxMarker;

   /** Connection line between marker and label */
   TGo4LabelConnector* fxConnector;

private:
    /** Flag to handle streamed connector from file correctly*/
    Bool_t fbIsConStreamed; //!


ClassDef(TGo4MarkerPainter,1)
};
#endif //TGO4MARKERPAINTER_H

