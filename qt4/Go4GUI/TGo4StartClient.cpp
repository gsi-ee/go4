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
   LineEditClientDir->setText(go4sett->getClientDir());
   LineEditClientExec->setText(go4sett->getClientExec());

   if (LineEditClientDir->text().isEmpty())
      LineEditClientDir->setText(QDir::currentPath());

   ExeModeCombo->setCurrentIndex(go4sett->getClientExeMode());

   bool isserver = go4sett->getClientIsServer();
   ServerModeCombo->setCurrentIndex(isserver ? 1 : 0);
#ifndef WIN32
   qt_selected->setEnabled(!isserver);
   if (isserver && ClientTermGroup->checkedId()==1)
      ClientTermGroup->button(2)->setChecked(true);
#endif
}

void TGo4StartClient::getResults()
{
   go4sett->setClientName(LineEditClientName->text());
   go4sett->setClientNode(LineEditClientNode->text());
   go4sett->setClientDir(LineEditClientDir->text());
   go4sett->setClientExec(LineEditClientExec->text());
   go4sett->setClientShellMode(ClientShellGroup->checkedId());
   go4sett->setClientTermMode(ClientTermGroup->checkedId());
   go4sett->setClientIsServer(ServerModeCombo->currentIndex()==1);
   go4sett->setClientExeMode(ExeModeCombo->currentIndex());
}

void TGo4StartClient::SelectDir()
{
   QFileDialog fd(this, "Select your working directory");
   fd.setFileMode(QFileDialog::DirectoryOnly);
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
   QStringList filters;
   if (ExeModeCombo->currentIndex()==0) {
      caption = "Select your analysis program";
#ifdef WIN32
      filters << "Executable (*.exe)";
#else
      filters << "Executable (*)";
//      filters << "Shell script (*.sh)";
#endif
   } else {
      caption = "Select your analysis library";
#ifdef WIN32
      filters << "Shared library (*.dll)";
#else
      filters << "Shared library (*.so)";
#endif
   }

   filters  << "Any files (*)";

   QFileDialog fd(this, caption);
   fd.setFileMode(QFileDialog::ExistingFile);
   fd.setNameFilters(filters);

   QString filename = LineEditClientExec->text();
   if (filename.length() > 0)
      fd.selectFile(filename);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   LineEditClientExec->setText(flst[0]);
}


void TGo4StartClient::ServerModeCombo_activated(int id)
{
#ifndef WIN32

   bool isserver = (id==1);

   qt_selected->setEnabled(!isserver);

   if (isserver) {
      if (ClientTermGroup->checkedId()==1)
        ClientTermGroup->button(2)->setChecked(true);
   }
#endif
}
