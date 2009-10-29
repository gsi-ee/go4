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

   ExeModeCombo->setCurrentIndex(go4sett->getClientExeMode());

   LineEditClientName->setText(go4sett->getClientName());
   LineEditClientNode->setText(go4sett->getClientNode());
   LineEditClientDir->setText(go4sett->getClientDir());
   LineEditClientExec->setText(go4sett->getClientExec());

   if (LineEditClientDir->text().isEmpty())
      LineEditClientDir->setText(QDir::currentPath());

   bool isserver = go4sett->getClientIsServer();
#ifdef WIN32
   isserver = false;
   ServerModeCombo->setEnabled(false);
#endif
   ServerModeCombo->setCurrentIndex(isserver ? 1 : 0);

#ifndef WIN32
   qt_selected->setEnabled(!isserver);
   if (isserver && ClientTermGroup->checkedId()==1)
      ClientTermGroup->button(2)->setChecked(true);
#endif
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
   go4sett->setClientShellMode(ClientShellGroup->checkedId());
   go4sett->setClientTermMode(ClientTermGroup->checkedId());
   go4sett->setClientIsServer(ServerModeCombo->currentIndex()==1);
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

   LineEditClientExec->setText(flst[0]);

   select_filer = 4;
   if (fd.selectedNameFilter()==exe1_filter) select_filer = 0; else
   if (fd.selectedNameFilter()==exe2_filter) select_filer = 1; else
   if (fd.selectedNameFilter()==dll1_filter) select_filer = 2; else
   if (fd.selectedNameFilter()==dll2_filter) select_filer = 3;
   fSelectedFilter = select_filer;

   if ((select_filer==0) || (select_filer==1)) ExeModeCombo->setCurrentIndex(0); else
   if ((select_filer==2) || (select_filer==3)) ExeModeCombo->setCurrentIndex(1);
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
