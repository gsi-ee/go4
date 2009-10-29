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

#ifndef TGO4CONDITIONPAINTER_H
#define TGO4CONDITIONPAINTER_H

#include "TGo4LabelPainter.h"

class TGo4Condition;

class TGo4ConditionPainter : public TGo4LabelPainter {
  public:
    TGo4ConditionPainter();
    TGo4ConditionPainter(const char* name, const char* title = 0);
    virtual ~TGo4ConditionPainter();

    /** Display condition with plain root canvas and in go4 viewpanel */
    virtual void PaintCondition(Option_t* opt="");

    /** Display condition with plain root canvas and in go4 viewpanel */
    virtual void UnPaintCondition(Option_t* opt="");

    /** Display statistics label for this condition with respect to working histogram */
    virtual void PaintLabel(Option_t* opt="");

    /** Not inline, because we overwrite this for polygon condition. */
    virtual void SetCondition(TGo4Condition* con);

  protected:
    /** Condition to be painted.*/
    TGo4Condition* fxCondition;

  ClassDef(TGo4ConditionPainter,6)
};

#endif //TGO4CONDITIONPAINTER_H
