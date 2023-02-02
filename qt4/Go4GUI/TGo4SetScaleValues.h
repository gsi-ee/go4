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

#ifndef TGO4SETSCALEVALUES_H
#define TGO4SETSCALEVALUES_H

#include <QWidget>
#include "ui_TGo4SetScaleValues.h"

class TGo4ViewPanel;
class TPad;

class TGo4SetScaleValues : public QWidget, public Ui::TGo4SetScaleValues
{
   Q_OBJECT

protected:
   bool fbDoingRefresh{false};

public:
   TGo4SetScaleValues( QWidget *parent = nullptr, const char *name = nullptr, Qt::WindowFlags fl = Qt::Widget );

   void refreshView(bool force);

public slots:

   virtual void panelSlot(TGo4ViewPanel* panel, TPad *pad, int signalid);

   virtual void ApplyValues();

   virtual void AutoscaleChanged(bool on);
};

#endif
