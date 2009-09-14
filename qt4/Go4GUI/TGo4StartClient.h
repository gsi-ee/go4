#ifndef TGO4STARTCLIENT_H
#define TGO4STARTCLIENT_H

#include <QDialog>
#include "ui_TGo4StartClient.h"

class QButtonGroup;

class TGo4StartClient : public QDialog, public Ui::TGo4StartClient
 {
     Q_OBJECT

 public:

	 TGo4StartClient(QWidget* parent = 0);
	 void getResults();

 public slots:

	virtual void SelectProg();

	virtual void ServerModeCombo_activated(int id);

   virtual void SelectDir();

   virtual void SwitchExpertMode();

   virtual void go4sys_cmb_activated(int id);
   virtual void go4sys_cmb_textchanged(const QString& txt);

   virtual void rootsys_cmb_activated(int id);
   virtual void rootsys_cmb_textchanged(const QString& txt);

protected:
	QButtonGroup* ClientShellGroup;
	QButtonGroup* ClientTermGroup;


 };

#endif
