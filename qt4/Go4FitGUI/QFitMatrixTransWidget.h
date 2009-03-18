#ifndef QFITMATRIXTRANSWIDGET_H
#define QFITMATRIXTRANSWIDGET_H

#include "QFitMatrixTransWidget.h"
#include "ui_QFitMatrixTransWidget.h"  

class TGo4FitMatrixTrans;

class QFitMatrixTransWidget : public QFitNamedWidget, public Ui::QFitMatrixTransWidget
 {
     Q_OBJECT


public:
    QFitMatrixTransWidget( QWidget* parent = 0, const char* name = 0 );


public slots:
    virtual TGo4FitMatrixTrans * GetMatrixTrans();
    virtual void FillSpecificData();
    virtual void NumAxisSpin_valueChanged( int );

 };
 
#endif
