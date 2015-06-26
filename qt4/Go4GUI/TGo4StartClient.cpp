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

#include "TGo4StartClient.h"

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include "TGo4QSettings.h"

TGo4StartClient::TGo4StartClient( QWidget* parent )
    : QDialog( parent )
{
   setupUi(this);

   setObjectName("Go4StartClient");

#ifdef WIN32
   // enable only exec and qtwindow in WIN32 mode
   go4sett->setClientShellMode(0);
   rsh_selected->setEnabled(false);
   ssh_selected->setEnabled(false);

   go4sett->setClientTermMode(1);
   xterm_selected->setEnabled(false);
   konsole_selected->setEnabled(false);
   go4sett->setClientNode("localhost");
   LineEditClientNode->setEnabled(false);
#endif

   fSelectedFilter = -1;

   ClientShellGroup = new QButtonGroup(this);
   ClientShellGroup->addButton(exec_selected, 0);
   ClientShellGroup->addButton(rsh_selected, 1);
   ClientShellGroup->addButton(ssh_selected, 2);
   ClientShellGroup->button(go4sett->getClientShellMode())->setChecked(true);

   ClientTermGroup = new QButtonGroup(this);
   ClientTermGroup->addButton(qt_selected, 1);
   ClientTermGroup->addButton(xterm_selected, 2);
   ClientTermGroup->addButton(konsole_selected, 3);
   ClientTermGroup->button(go4sett->getClientTermMode())->setChecked(true);

   LineEditClientName->setText(go4sett->getClientName());
   LineEditClientNode->setText(go4sett->getClientNode());
   PortNumber->setValue(go4sett->getClientPort());
   LineEditClientDir->setText(go4sett->getClientDir());
   LineEditClientExec->setText(go4sett->getClientExec());
   LineEditArgs->setText(go4sett->getClientArgs());

   if (LineEditClientDir->text().isEmpty())
      LineEditClientDir->setText(QDir::currentPath());

   int mode = go4sett->getClientIsServer();  // 0 - client, 1 - server, 2 - http

#ifdef WIN32
   if (mode == 1) mode = 2;

   QModelIndex index = ServerModeCombo->model()->index(1, 0);
   // This is the effective 'disable' flag
   QVariant v(0);
   // the magic
   ServerModeCombo->model()->setData(index, v, Qt::UserRole - 1);
#else
   qt_selected->setEnabled(mode!=1);

   if ((mode==1) && ClientTermGroup->checkedId()==1)
      ClientTermGroup->button(2)->setChecked(true);
#endif

   ServerModeCombo->setCurrentIndex(mode);
   PortNumber->setEnabled(mode == 2);

   ExeModeCombo->setCurrentIndex(mode==2 ? 1 : go4sett->getClientExeMode());


}

void TGo4StartClient::ExeMode_changed(int id)
{
   LineEditClientExec->setText(go4sett->getClientExec(id));
}

void TGo4StartClient::getResults()
{
   go4sett->setClientName(LineEditClientName->text());
   go4sett->setClientNode(LineEditClientNode->text());
   go4sett->setClientDir(LineEditClientDir->text());
   go4sett->setClientExeMode(ExeModeCombo->currentIndex());
   go4sett->setClientExec(LineEditClientExec->text());
   go4sett->setClientArgs(LineEditArgs->text());
   go4sett->setClientShellMode(ClientShellGroup->checkedId());
   go4sett->setClientTermMode(ClientTermGroup->checkedId());
   go4sett->setClientIsServer(ServerModeCombo->currentIndex());
   go4sett->setClientConnectMode(ServerModeCombo->currentIndex()>0 ? ServerModeCombo->currentIndex() - 1 : 0);
   go4sett->setClientPort(PortNumber->value());
}

void TGo4StartClient::SelectDir()
{
   QFileDialog fd(this, "Select your working directory");
   fd.setFileMode(QFileDialog::DirectoryOnly);
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
   fd.setOption(QFileDialog::DontUseNativeDialog);
#endif
   //< JAM also do not use native dialog here to be consistent with lib dialog, see below
   QString dir = LineEditClientDir->text();
   if (dir.isEmpty() || !QDir(dir).exists())
      dir = QDir::currentPath();
   fd.setDirectory(dir);

   if (fd.exec() != QDialog::Accepted ) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   LineEditClientDir->setText(flst[0]);

   QString fileName = LineEditClientExec->text();
   if (fileName.length()==0) return;

   QString exeDirPath = QFileInfo(fileName).path();
   if (exeDirPath.isEmpty() || ((exeDirPath == QString(".")) && (fileName[0]!='.'))) {
      fileName = QFileInfo(QDir(flst[0]), fileName).filePath();
      LineEditClientExec->setText(fileName);
   }
}


void TGo4StartClient::SelectProg()
{
   const char* caption = 0;

#ifdef WIN32
   QString exe1_filter("User analysis (MainUserAnalysis.exe)");
   QString exe2_filter("Any executable (*.exe)");
   QString dll1_filter("User library (libGo4UserAnalysis.dll)");
   QString dll2_filter("Any shared library (*.dll)");
   QString any_filter("Any files (*.*)");
#else
   QString exe1_filter("User analysis (MainUserAnalysis)");
   QString exe2_filter("Any executable (*)");
   QString dll1_filter("User library (libGo4UserAnalysis.so)");
   QString dll2_filter("Any shared library (*.so)");
   QString any_filter("Any files (*)");
#endif

   QStringList filters;
   filters << exe1_filter << exe2_filter << dll1_filter << dll2_filter << any_filter;

   QFileDialog fd(this, caption);
   fd.setFileMode(QFileDialog::ExistingFile);
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
   fd.setOption(QFileDialog::DontUseNativeDialog);
#endif
   //< JAM: native dialog may suppress the chosen filer when displayed and use always first entry in list!
   fd.setNameFilters(filters);
   int select_filer = fSelectedFilter;
   if (select_filer<0)
      select_filer= (ExeModeCombo->currentIndex()==0) ? 0 : 2;
   switch (select_filer) {
      case 0: fd.selectNameFilter(exe1_filter); break;
      case 1: fd.selectNameFilter(exe2_filter); break;
      case 2: fd.selectNameFilter(dll1_filter); break;
      case 3: fd.selectNameFilter(dll2_filter); break;
      default: fd.selectNameFilter(any_filter); break;
   }

   QString filename = LineEditClientExec->text();
   if (filename.length() > 0)
      fd.selectFile(filename);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   select_filer = 4;
   if (fd.selectedNameFilter()==exe1_filter) select_filer = 0; else
   if (fd.selectedNameFilter()==exe2_filter) select_filer = 1; else
   if (fd.selectedNameFilter()==dll1_filter) select_filer = 2; else
   if (fd.selectedNameFilter()==dll2_filter) select_filer = 3;
   fSelectedFilter = select_filer;

   if ((select_filer==0) || (select_filer==1)) ExeModeCombo->setCurrentIndex(0); else
   if ((select_filer==2) || (select_filer==3)) ExeModeCombo->setCurrentIndex(1);

   LineEditClientExec->setText(flst[0]);
}


void TGo4StartClient::ServerModeCombo_activated(int id)
{
   bool windows = false;

#ifdef WIN32
   if (id==1) {
      ServerModeCombo->setCurrentIndex(2);
      id = 2;
   }
   windows = true;
#endif

   ExeModeCombo->setEnabled(id<2);
   if (id==2) ExeModeCombo->setCurrentIndex(1);
   bool isserver = (id>0);
   qt_selected->setEnabled(id!=1);
   PortNumber->setEnabled(id==2);
   if (id==1) { // if Qt was selected, select Xterm
      if (ClientTermGroup->checkedId()==1)
        ClientTermGroup->button(2)->setChecked(true);
   }
   else { // if Xterm was selected, select Qt
	  if (ClientTermGroup->checkedId()==2)
		 ClientTermGroup->button(1)->setChecked(true);
   }
}
