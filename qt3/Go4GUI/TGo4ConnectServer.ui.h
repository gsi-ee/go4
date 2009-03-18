void TGo4ConnectServer::init()
{
   ClientNode->setText(go4sett->getClientNode());
   PortNumber->setValue(go4sett->getClientPort());
   ModeCombo->setCurrentItem(go4sett->getClientControllerMode());
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
   go4sett->setClientControllerMode(ModeCombo->currentItem());
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


