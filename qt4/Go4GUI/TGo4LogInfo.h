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

#ifndef TGO4LOGINFO_H
#define TGO4LOGINFO_H

#include "QGo4Widget.h"
#include "ui_TGo4LogInfo.h"

#include <QDateTime>

class TGo4LogInfo : public QGo4Widget, public Ui::TGo4LogInfo
{
   Q_OBJECT

public:
   TGo4LogInfo(QWidget *parent = nullptr, const char *name = nullptr);

   void WorkWithInfo(TGo4Slot *slot);

   void linkedObjectUpdated(const char *linkname, TObject * obj) override;

   void AddMessage(const QDateTime& dt, int level, QString msg);

public slots:

   virtual void ClearLogInfo();

   virtual void SaveLogInfo();

};

#endif
