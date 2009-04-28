#ifndef TGO4MACRODIALOG_H
#define TGO4MACRODIALOG_H

#include <QDialog>
#include "ui_TGo4MacroDialog.h"

class TGo4MacroDialog : public QDialog, public Ui::TGo4MacroDialog
 {
     Q_OBJECT

 public:

	 TGo4MacroDialog( QWidget* parent = 0 );

	const QString& getCommand();

 public slots:



	virtual void setCommand( const QString & selection );

 protected:
    QString fxCommand;


};

#endif

