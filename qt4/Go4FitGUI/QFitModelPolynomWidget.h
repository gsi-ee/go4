#ifndef QFITMODELPOLYNOMWIDGET_H
#define QFITMODELPOLYNOMWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelPolynomWidget.h"  


#include <qvariant.h>
#include <QFitModelWidget.h>

class TGo4FitModelPolynom;



class QFitModelPolynomWidget : public QFitModelWidget, public Ui::QFitModelPolynomWidget
 {

    Q_OBJECT

public:
    QFitModelPolynomWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitModelPolynom * GetPolynom();
    virtual void FillSpecificData();
    virtual void AxisSpin_valueChanged( int );


};

#endif // QFITMODELPOLYNOMWIDGET_H

