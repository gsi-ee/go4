#ifndef QFITMODELGAUSS1WIDGET_H
#define QFITMODELGAUSS1WIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelGauss1Widget.h"  


class TGo4FitModelGauss1;

class QFitModelGauss1Widget : public QFitModelWidget, public Ui::QFitModelGauss1Widget
 {
    Q_OBJECT

public:
    QFitModelGauss1Widget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitModelGauss1 * GetGauss1();
    virtual void FillSpecificData();
    virtual void AxisNumSpin_valueChanged( int );


};



#endif