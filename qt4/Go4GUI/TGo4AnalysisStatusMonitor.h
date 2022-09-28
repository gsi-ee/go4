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

#ifndef TGO4ANALYSISSTATUSMONITOR_H
#define TGO4ANALYSISSTATUSMONITOR_H

#include "QGo4Widget.h"
#include "ui_TGo4AnalysisStatusMonitor.h"

class QMovie;

class TGo4AnalysisStatusMonitor : public QGo4Widget, public Ui::TGo4AnalysisStatusMonitor
{
   Q_OBJECT

protected:

   QMovie* fxRunMovie{nullptr};

public:
   TGo4AnalysisStatusMonitor(QWidget *parent = nullptr, const char *name = nullptr);

   virtual ~TGo4AnalysisStatusMonitor();

   virtual void WorkWithRatemeter(TGo4Slot* slot);

   void linkedObjectUpdated(const char *linkname, TObject *obj) override;

   void linkedObjectRemoved(const char *linkname) override;
};

#endif
