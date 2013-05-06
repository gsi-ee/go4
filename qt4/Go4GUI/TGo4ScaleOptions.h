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

#ifndef TGO4SCALEOPTIONS_H
#define TGO4SCALEOPTIONS_H

#include "QGo4Widget.h"
#include "ui_TGo4ScaleOptions.h"

class TGo4ScaleOptions : public QGo4Widget, public Ui::TGo4ScaleOptions
 {
     Q_OBJECT

 public:
     TGo4ScaleOptions(QWidget *parent = 0, const char* name=0);
    void MoveScale(int xaction, int yaction, int zaction);

 public slots:

   virtual void SlotShiftL();

   virtual void SlotShiftR();

   virtual void SlotShiftU();

   virtual void SlotShiftD();

   virtual void SlotShiftZU();

   virtual void SlotShiftZD();

   virtual void SlotExpandX();

   virtual void SlotExpandY();

   virtual void SlotExpandZ();

   virtual void SlotUExpandX();

   virtual void SlotUExpandY();

   virtual void SlotUExpandZ();

   virtual void resizeall();

   virtual void SetScaleValues();

   virtual void AutoZoomScales();

 };

#endif
