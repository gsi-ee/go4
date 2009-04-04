#ifndef QFITMODELGAUSSNWIDGET_H
#define QFITMODELGAUSSNWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelGaussNWidget.h"


class TGo4FitModelGaussN;


class QFitModelGaussNWidget : public QFitModelWidget, public Ui::QFitModelGaussNWidget
 {
	Q_OBJECT

public:
    QFitModelGaussNWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
   virtual TGo4FitModelGaussN * GetGaussN();
   virtual void SetAxisTable();
   virtual void FillSpecificData();
   virtual void AxisNumberSpin_valueChanged( int );
   virtual void AxisTable_valueChanged( int, int );


};

#endif // QFITMODELGAUSSNWIDGET_H






