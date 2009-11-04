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

#ifndef TGO4CONNECTSERVER_H
#define TGO4CONNECTSERVER_H

#include <QDialog>
#include "ui_TGo4ConnectServer.h"

class TGo4ConnectServer : public QDialog, public Ui::TGo4ConnectServer
 {
     Q_OBJECT

 public:
   TGo4ConnectServer( QWidget* parent = 0 );

   void setPassword(const char* pass);

   QString getInput();


 public slots:



   virtual void DefaultPassCheck_toggled( bool on );

 };


#endif
