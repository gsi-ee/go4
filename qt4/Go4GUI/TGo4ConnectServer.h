#ifndef TGO4CONNECTSERVER_H
#define TGO4CONNECTSERVER_H

#include <QDialog>
#include "ui_TGo4ConnectServer.h"  

class TGo4ConnectServer : public QDialog, public Ui::TGo4ConnectServer
 {
     Q_OBJECT

 public:
	TGo4ConnectServer( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );

	void setPassword(const char* pass);
	
	QString getInput();


 public slots:

	
	
	virtual void DefaultPassCheck_toggled( bool on );

 };


#endif