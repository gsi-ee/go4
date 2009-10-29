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

#include "TGo4ConnectServer.h"
#include "TGo4QSettings.h"

TGo4ConnectServer::TGo4ConnectServer( QWidget* parent )
   : QDialog( parent )
{
   setObjectName("Go4ConnectServer");
   setupUi(this);

   ClientNode->setText(go4sett->getClientNode());
   PortNumber->setValue(go4sett->getClientPort());
   ModeCombo->setCurrentIndex(go4sett->getClientControllerMode());
   bool def = go4sett->getClientDefaultPass();
   DefaultPassCheck->setChecked(def);
   PasswordEdit->setEnabled(!def);
}

void TGo4ConnectServer::setPassword(const char* pass)
{
   if (PasswordEdit->isEnabled())
     PasswordEdit->setText(pass);
}

QString TGo4ConnectServer::getInput()
{
   go4sett->setClientNode(ClientNode->text());
   go4sett->setClientPort(PortNumber->value());
   go4sett->setClientControllerMode(ModeCombo->currentIndex());
   bool def = DefaultPassCheck->isChecked();
   go4sett->setClientDefaultPass(def);
   QString res;
   if (!def) res = PasswordEdit->text();
   return res;
}

void TGo4ConnectServer::DefaultPassCheck_toggled( bool on )
{
   PasswordEdit->setEnabled(!on);
}


