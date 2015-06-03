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

   ConnectionCombo->setCurrentIndex(go4sett->getClientConnectMode());
   bool canHaveOtherAccounts=(go4sett->getClientConnectMode()!=0);
   bool def = go4sett->getClientDefaultPass();
   DefaultPassCheck->setChecked(def);
   PasswordEdit->setEnabled(!def);
   AccountEdit->setEnabled(canHaveOtherAccounts);
   AccountEdit->setVisible(canHaveOtherAccounts);
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
   go4sett->setClientConnectMode(ConnectionCombo->currentIndex());
   go4sett->setClientAccountName(AccountEdit->text());
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

void TGo4ConnectServer::SelectAccount_changed( int index )
{

    switch(index)
    {
      case 0:
      default:
        AccountEdit->setText("observer");
        break;
      case 1:
        AccountEdit->setText("controller");
             break;
      case 2:
        AccountEdit->setText("admin");
             break;
      case 3:
        AccountEdit->setText(fLastAccount);
        break;
    };

    AccountEdit->setEnabled(index==3);

}

void TGo4ConnectServer::SelectConnection_changed ( int index )
{
  AccountEdit->setEnabled(index==1);
  AccountEdit->setVisible(index==1);

}
