#ifndef TGO4DIVIDEPAD_H
#define TGO4DIVIDEPAD_H

#include <QWidget>
#include "ui_TGo4DividePad.h"  

class TGo4DividePad : public QWidget, public Ui::TGo4DividePad
 {
     Q_OBJECT

 public:
	 TGo4DividePad( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );

 public slots:

	virtual void DivideCanvas();

};

#endif