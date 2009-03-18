#ifndef TGO4STARTCLIENT_H
#define TGO4STARTCLIENT_H

#include <QDialog>
#include "ui_TGo4StartClient.h"  

class TGo4StartClient : public QDialog, public Ui::TGo4StartClient
 {
     Q_OBJECT

 public:

	 TGo4StartClient( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
	 void getResults();

 public slots:

	
	
	virtual void SelectDir();
	
	virtual void SelectProg();
	
	virtual void ServerModeCombo_activated(int id);

 };
 
#endif