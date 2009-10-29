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

#ifndef TGO4WINCONDPAINTER_H
#define TGO4WINCONDPAINTER_H

#include "TGo4ConditionPainter.h"

class TGo4WinCondView;

class TGo4WinCondPainter : public TGo4ConditionPainter {
  public:
     TGo4WinCondPainter();
     TGo4WinCondPainter(const char* name, const char* title = "Go4 window condition");
     virtual ~TGo4WinCondPainter();

     /** Display condition with plain root canvas and in go4 viewpanel */
     virtual void PaintCondition(Option_t* opt="");

     /** Erase condition view  */
     virtual void UnPaintCondition(Option_t* opt="");

        /** pop all components to the front of the pad*/
    virtual void DisplayToFront(Option_t* opt="");



  private:

    /** Window marker box. */
    TGo4WinCondView* fxBox;

    ClassDef(TGo4WinCondPainter,6)
};

#endif //TGO4WINCONDPAINTER_H
