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

#ifndef TGO4STARTCLIENT_H
#define TGO4STARTCLIENT_H

#include <QDialog>
#include "ui_TGo4StartClient.h"

class QButtonGroup;

class TGo4StartClient : public QDialog, public Ui::TGo4StartClient {
   Q_OBJECT

   public:

      TGo4StartClient(QWidget* parent = 0);
      void getResults();

   public slots:

      virtual void SelectDir();

      virtual void SelectProg();

      virtual void ServerModeCombo_activated(int);

      virtual void ExeMode_changed(int);

   protected:
      QButtonGroup* ClientShellGroup;
      QButtonGroup* ClientTermGroup;
      int fSelectedFilter;
};

#endif
