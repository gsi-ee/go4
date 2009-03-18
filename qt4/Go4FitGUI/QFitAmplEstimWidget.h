#ifndef QFITAMPLESTIMWIDGET_H
#define QFITAMPLESTIMWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitAmplEstimWidget.h"  

class TGo4FitAmplEstimation;

class QFitAmplEstimWidget : public QFitNamedWidget, public Ui::QFitAmplEstimWidget
 {
     Q_OBJECT

 public:
    QFitAmplEstimWidget( QWidget* parent = 0, const char* name = 0 );

 public slots:
    virtual TGo4FitAmplEstimation * GetAmplEstim();
    virtual void FillSpecificData();
    virtual void Iterations_valueChanged( int );

 };

#endif