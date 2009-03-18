#ifndef TGO4STYLE_H
#define TGO4STYLE_H

#include <QWidget>
#include "ui_TGo4Style.h"  

class TGo4Style : public QWidget, public Ui::TGo4Style
 {
     Q_OBJECT

 public:
     
	 TGo4Style( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );

 public slots:

	virtual void SetPadColor();
	
	virtual void SetPalette( int t );

 };
 
#endif