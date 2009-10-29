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

#ifndef TGO4CONDARRAYPAINTER_H
#define TGO4CONDARRAYPAINTER_H

#include "TGo4ConditionPainter.h"

class TGo4CondArrayPainter : public TGo4ConditionPainter {
  public:
    TGo4CondArrayPainter();
    virtual ~TGo4CondArrayPainter();

    TGo4CondArrayPainter(const char* name);

   /** Display condition with plain root canvas and in go4 viewpanel */
   virtual void PaintCondition(Option_t* opt="");

   /** Erase condition view */
   virtual void UnPaintCondition(Option_t* opt="");

   /** To suppress the label of the array, we overwrite method here. */
   virtual void PaintLabel(Option_t* opt="");

  ClassDef(TGo4CondArrayPainter,6)
};

#endif //TGO4CONDARRAYPAINTER_H
