#include "TGo4StartClient.h"
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include "TGo4QSettings.h"

#include "Riostream.h"

const char* NonSpecifiedString = "-- specify --";
const char* ExpertText = "<< &Expert";
const char* NormalText = ">> &Normal";

TGo4StartClient::TGo4StartClient( QWidget* parent )
    : QDialog( parent )
{
	setupUi(this);

	setObjectName("Go4StartClient");

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

   bool isserver = go4sett->getClientIsServer();
   ServerModeCombo->setCurrentIndex(isserver ? 1 : 0);
   qt_selected->setEnabled(!isserver);
   if (isserver && ClientTermGroup->checkedId()==1)
      ClientTermGroup->button(2)->setChecked(true);

   bool isexpert = go4sett->getClientExpert();

   ExpertBox->setVisible(isexpert);
   PushButtonExpertMode->setText(isexpert ? NormalText : ExpertText);

   QString path = go4sett->getClientGo4SysCustom();
   if (path.length()==0) path = NonSpecifiedString;
   go4sys_cmb->setItemText(3, path);
   go4sys_cmb->setCurrentIndex(go4sett->getClientGo4SysMode());

   path = go4sett->getClientRootSysCustom();
   if (path.length()==0) path = NonSpecifiedString;
   rootsys_cmb->setItemText(3, path);
   rootsys_cmb->setCurrentIndex(go4sett->getClientRootSysMode());

   path_edt->setText(go4sett->getClientAddPath());
   ldpath_edt->setText(go4sett->getClientAddLDPath());

   adjustSize();

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

   go4sett->setClientExpert(PushButtonExpertMode->text() == NormalText);

   go4sett->setClientGo4SysMode(go4sys_cmb->currentIndex());
   QString path = go4sys_cmb->itemText(3);
   if (path==NonSpecifiedString) path = "";
   go4sett->setClientGo4SysCustom(path);

   go4sett->setClientRootSysMode(rootsys_cmb->currentIndex());
   path = rootsys_cmb->itemText(3);
   if (path==NonSpecifiedString) path = "";
   go4sett->setClientRootSysCustom(path);

   go4sett->setClientAddPath(path_edt->text());
   go4sett->setClientAddLDPath(ldpath_edt->text());
}

void TGo4StartClient::SelectDir()
{
   QFileDialog fd(this, "Select your working dir");
   fd.setFileMode(QFileDialog::DirectoryOnly);
   QString dir = LineEditClientDir->text();
   if (dir.isEmpty() || !QDir(dir).exists())
         dir = QDir::currentPath();
   fd.setDirectory(dir);

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

void TGo4StartClient::SwitchExpertMode()
{
   if (ExpertBox->isVisible()) {
      ExpertBox->setVisible(false);
      PushButtonExpertMode->setText(ExpertText);
   } else {
      ExpertBox->setVisible(true);
      PushButtonExpertMode->setText(NormalText);
   }
   resize(width(), 100);
   adjustSize();
}


void TGo4StartClient::go4sys_cmb_activated(int id)
{
   if (id<3)
      go4sys_cmb->setEditable(false);
   else {
      if (go4sys_cmb->currentText() == QString(NonSpecifiedString))
         go4sys_cmb->setItemText(id, "");
      if (!go4sys_cmb->isEditable()) {
         go4sys_cmb->setEditable(true);
         connect(go4sys_cmb, SIGNAL(editTextChanged(const QString &)), this, SLOT(go4sys_cmb_textchanged(const QString &)));
      }
   }
}

void TGo4StartClient::go4sys_cmb_textchanged(const QString& txt)
{
//   cout << "text changed " << txt.toAscii().constData() << endl;

   if ((go4sys_cmb->currentIndex() == 3) && (go4sys_cmb->itemText(3)!=txt))
      go4sys_cmb->setItemText(3, txt);
}


void TGo4StartClient::rootsys_cmb_activated(int id)
{
   if (id<3)
      rootsys_cmb->setEditable(false);
   else {
      if (rootsys_cmb->currentText() == QString(NonSpecifiedString))
         rootsys_cmb->setItemText(id,"");
      if (!rootsys_cmb->isEditable()) {
         rootsys_cmb->setEditable(true);
         connect(rootsys_cmb, SIGNAL(editTextChanged(const QString &)), this, SLOT(rootsys_cmb_textchanged(const QString &)));
      }
   }
}

void TGo4StartClient::rootsys_cmb_textchanged(const QString& txt)
{
   if ((rootsys_cmb->currentIndex() == 3) && (rootsys_cmb->itemText(3)!=txt))
      rootsys_cmb->setItemText(3, txt);
}
