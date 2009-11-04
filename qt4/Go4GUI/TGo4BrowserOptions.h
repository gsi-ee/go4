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

#ifndef TGO4BROWSEROPTIONS_H
#define TGO4BROWSEROPTIONS_H

#include "QGo4Widget.h"
#include "ui_TGo4BrowserOptions.h"

class TGo4BrowserOptions : public QGo4Widget, public Ui::TGo4BrowserOptions
{
   Q_OBJECT

public:

   TGo4BrowserOptions(QWidget *parent = 0, const char* name=0);

   virtual void StartWorking();

   virtual void UpdateView();

   virtual void linkedObjectUpdated(const char* linkname, TObject* obj);

public slots:

   virtual void RefreshBtn_clicked();

   virtual void ClearBtn_clicked();

   virtual void StartMonitorBtn_clicked();

   virtual void StopMonitorBtn_clicked();

   virtual void FilterBox_activated(int indx);

};

#endif
