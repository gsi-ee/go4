#ifndef QFITMODELGAUSSNWIDGET_H
#define QFITMODELGAUSSNWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelGaussNWidget.h"  


class TGo4FitModelGaussN;


class QFitModelGaussNWidget : public QFitModelWidget, public Ui::QFitModelGaussNWidget
 {

public:
    QFitModelGaussNWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitModelGaussN * GetGaussN();
    virtual void FillSpecificData();
    virtual void SetAxisTable();
    virtual void AxisNumberSpin_valueChanged( int );
    virtual void AxisTable_valueChanged( int, int );


};

#endif // QFITMODELGAUSSNWIDGET_H






