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

#ifndef TGO4EVENTINFO_H
#define TGO4EVENTINFO_H

#include "QGo4Widget.h"
#include "ui_TGo4EventInfo.h"

class TGo4EventInfo : public QGo4Widget, public Ui::TGo4EventInfo {
   Q_OBJECT

public:
   TGo4EventInfo(QWidget *parent = nullptr, const char *name = nullptr);

   bool IsAcceptDrag(const char *itemname, TClass *cl, int kind) override;

   void DropItem(const char *itemname, TClass *cl, int kind) override;

   void WorkWithEvent(const char *itemname);

   void ResetWidget() override;

   void linkedObjectUpdated(const char *linkname, TObject *obj) override;

   void linkedObjectRemoved(const char *linkname) override;

public slots:

   virtual void RefreshClicked();

   virtual void PrintEventClicked();

   virtual void MbsButton_clicked();

};

#endif
