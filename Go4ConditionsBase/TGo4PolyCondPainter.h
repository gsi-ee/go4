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

#ifndef TGO4POLYCONDPAINTER_H
#define TGO4POLYCONDPAINTER_H

#include "TGo4ConditionPainter.h"

class TGo4PolyCond;
class TGo4PolyCondView;

class TGo4PolyCondPainter : public TGo4ConditionPainter {
   public:
      TGo4PolyCondPainter();
      TGo4PolyCondPainter(const char *name, const char* title = "Go4 polygon condition");
      virtual ~TGo4PolyCondPainter();

      /** Display condition with plain root canvas and in go4 viewpanel */
      void PaintCondition(Option_t* opt="") override;

      /** Erase condition view */
      void UnPaintCondition(Option_t* opt="") override;

        /** pop all components to the front of the pad*/
      void DisplayToFront(Option_t* opt="") override;

      void SetCondition(TGo4Condition* con) override;

   private:
      /** Cut used for polygon display. This is buffered copy of condition cutg*/
      TGo4PolyCondView *fxCutView{nullptr}; //!

   ClassDefOverride(TGo4PolyCondPainter,6)
};

#endif //TGO4POLYCONDPAINTER_H
