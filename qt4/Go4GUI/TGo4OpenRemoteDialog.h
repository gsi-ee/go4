#ifndef TGO4OPENREMOTEDIALOG_H
#define TGO4OPENREMOTEDIALOG_H

#include <QDialog>
#include "ui_TGo4OpenRemoteDialog.h"  

class TGo4OpenRemoteDialog : public QDialog, public Ui::TGo4OpenRemoteDialog
 {
     Q_OBJECT

 public:

	 TGo4OpenRemoteDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );	 

	virtual ~TGo4OpenRemoteDialog(); 
	
	const QString& GetFileName();
	
	const QString& GetFileType();	
	
	const QString& GetServer();
	
 public slots:


	virtual void TypeSelectorBox_activated( const QString & tname);
	
	virtual void ServerEdit_textChanged( const QString &  txt);
	
	virtual void FileNameEdit_textChanged( const QString & txt);

 protected:
    QString fxServer;
    QString fxFile;
    QString fxType;

	
 };	
 
#endif