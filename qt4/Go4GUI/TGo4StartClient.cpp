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

	ClientShellGroup = new QButtonGroup(this);
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

   bool isserver = go4sett->getClientIsServer();
   ServerModeCombo->setCurrentIndex(isserver ? 1 : 0);
   qt_selected->setEnabled(!isserver);
   if (isserver && ClientTermGroup->checkedId()==1)
      ClientTermGroup->button(2)->setChecked(true);
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
}

void TGo4StartClient::SelectDir()
{
   QFileDialog fd(this, "Select your working dir");
   fd.setFileMode(QFileDialog::DirectoryOnly);

   if (fd.exec() != QDialog::Accepted ) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString fileName = flst[0];
   LineEditClientDir->setText(fileName);
   QDir::setCurrent(fileName);
}

void TGo4StartClient::SelectProg()
{
   QFileDialog fd(this, "Select your analysis program");
   fd.setFileMode(QFileDialog::ExistingFile);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QFileInfo fi(flst[0]);
   LineEditClientExec->setText(fi.fileName());
   LineEditClientDir->setText(fd.directory().path());
   QDir::setCurrent(fd.directory().path());
}


void TGo4StartClient::ServerModeCombo_activated(int id)
{
   bool isserver = (id==1);

   qt_selected->setEnabled(!isserver);

   if (isserver) {
     if (ClientTermGroup->checkedId()==1)
       ClientTermGroup->button(2)->setChecked(true);
   }
}
