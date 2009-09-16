void TGo4StartClient::init()
{
   LineEditClientName->setText(go4sett->getClientName());
   LineEditClientNode->setText(go4sett->getClientNode());
   LineEditClientDir->setText(go4sett->getClientDir());
   LineEditClientExec->setText(go4sett->getClientExec());
   ClientShellGroup->setButton(go4sett->getClientShellMode());
   ClientTermGroup->setButton(go4sett->getClientTermMode());

   bool isserver = go4sett->getClientIsServer();
   ServerModeCombo->setCurrentItem(isserver ? 1 : 0);
   qt_selected->setEnabled(!isserver);
   if (isserver && ClientTermGroup->selectedId()==1)
      ClientTermGroup->setButton(2);
}

void TGo4StartClient::getResults()
{
   go4sett->setClientName(LineEditClientName->text());
   go4sett->setClientNode(LineEditClientNode->text());
   go4sett->setClientDir(LineEditClientDir->text());
   go4sett->setClientExec(LineEditClientExec->text());
   go4sett->setClientShellMode(ClientShellGroup->selectedId());
   go4sett->setClientTermMode(ClientTermGroup->selectedId());
   go4sett->setClientIsServer(ServerModeCombo->currentItem()==1);
}

void TGo4StartClient::SelectDir()
{
   QFileDialog fd(this, "file name", TRUE);
   fd.setMode(QFileDialog::DirectoryOnly);
   fd.setName("Select your working directory");
   fd.setCaption("Select your working directory");
   if (LineEditClientDir->text().length()>0)
      fd.setDir(LineEditClientDir->text());

   if (fd.exec() != QDialog::Accepted ) return;

   QString dirName = fd.selectedFile();
   LineEditClientDir->setText(dirName);

   QString fileName = LineEditClientExec->text();
   if (fileName.length()==0) return;

   QString exeDirPath = QFileInfo(fileName).dirPath();
   if (exeDirPath.isEmpty() || ((exeDirPath == QString(".")) && (fileName[0]!='.'))) {
      fileName = QFileInfo(QDir(dirName), fileName).filePath();
      LineEditClientExec->setText(fileName);
   }
}

void TGo4StartClient::SelectProg()
{
   QFileDialog fd(this, "file name", TRUE);
   fd.setMode(QFileDialog::ExistingFile);
   fd.setName("Select your analysis program");
   fd.setCaption("Select your analysis program");

   if (LineEditClientExec->text().length()>0)
      fd.setSelection(LineEditClientExec->text());

   if (fd.exec() != QDialog::Accepted) return;

   LineEditClientExec->setText(fd.selectedFile());
}


void TGo4StartClient::ServerModeCombo_activated(int id)
{
   bool isserver = (id==1);

   qt_selected->setEnabled(!isserver);

   if (isserver) {
     if (ClientTermGroup->selectedId()==1)
       ClientTermGroup->setButton(2);
   }
}
