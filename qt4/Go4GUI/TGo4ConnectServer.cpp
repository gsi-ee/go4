#include "TGo4ConnectServer.h"
#include "TGo4QSettings.h"



TGo4ConnectServer::TGo4ConnectServer( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections
			// from ui file to the setupUI

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


