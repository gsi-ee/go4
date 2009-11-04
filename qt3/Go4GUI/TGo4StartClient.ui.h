// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

void TGo4StartClient::init()
{
   LineEditClientName->setText(go4sett->getClientName());
   LineEditClientNode->setText(go4sett->getClientNode());
   LineEditClientDir->setText(go4sett->getClientDir());
   ExeModeCombo->setCurrentItem(go4sett->getClientExeMode());
   LineEditClientExec->setText(go4sett->getClientExec());
   LineEditClientArgs->setText(go4sett->getClientArgs());
   ClientShellGroup->setButton(go4sett->getClientShellMode());
   ClientTermGroup->setButton(go4sett->getClientTermMode());

   bool isserver = go4sett->getClientIsServer();
   ServerModeCombo->setCurrentItem(isserver ? 1 : 0);
   qt_selected->setEnabled(!isserver);
   if (isserver && ClientTermGroup->selectedId()==1)
      ClientTermGroup->setButton(2);
}

void TGo4StartClient::ExeModeCombo_activated(int mode)
{
   LineEditClientExec->setText(go4sett->getClientExec(mode));
}

void TGo4StartClient::getResults()
{
   go4sett->setClientName(LineEditClientName->text());
   go4sett->setClientNode(LineEditClientNode->text());
   go4sett->setClientDir(LineEditClientDir->text());
   go4sett->setClientExeMode(ExeModeCombo->currentItem());
   go4sett->setClientExec(LineEditClientExec->text());
   go4sett->setClientArgs(LineEditClientArgs->text());
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

   QString filters;

   if (ExeModeCombo->currentItem()==0) {
      fd.setCaption("Select your analysis program");
      filters="User analysis (MainUserAnalysis);;"
              "Any executable (*);;";
   } else {
      fd.setCaption("Select your analysis library");
      filters="User library (libGo4UserAnalysis.so);;"
              "Any shared library (*.so);;"
              "Any file (*);;";
   }

   fd.setFilters(filters);
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
