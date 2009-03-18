#ifndef QFITTERWIDGET_H
#define QFITTERWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitterWidget.h"  

class TGo4Fitter;

class QFitterWidget : public QFitNamedWidget, public Ui::QFitterWidget
 {
    Q_OBJECT

public:
    QFitterWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4Fitter * GetFitter();
    virtual void FillSpecificData();
    virtual void FitFunctionTypeCmb_activated( int typ );


};

#endif // QFITTERWIDGET_H


