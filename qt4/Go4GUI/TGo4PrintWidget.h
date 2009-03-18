#ifndef TGO4PRINTWIDGET_H
#define TGO4PRINTWIDGET_H

#include <QDialog>
#include "ui_TGo4PrintWidget.h"  

class TGo4PrintWidget : public QDialog, public Ui::TGo4PrintWidget
 {
     Q_OBJECT

 public:
	 TGo4PrintWidget( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );

 public slots:

	QString GetPrinter();

	QString GetPrintProg();


 };
 
#endif