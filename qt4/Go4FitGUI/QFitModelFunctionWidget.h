#ifndef QFITMODELFUNCTIONWIDGET_H
#define QFITMODELFUNCTIONWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelFunctionWidget.h"  

class TGo4FitModelFunction;

class QFitModelFunctionWidget : public QFitModelWidget, public Ui::QFitModelFunctionWidget
 {
    Q_OBJECT

public:
    QFitModelFunctionWidget( QWidget* parent = 0, const char* name = 0 );


public slots:
    virtual TGo4FitModelFunction * GetFunction();
    virtual void FillSpecificData();
    virtual void LibNameEdt_textChanged( const QString & );
    virtual void FuncNameEdt_textChanged( const QString & );
    virtual void NumParSpin_valueChanged( int );

};

#endif // QFITMODELFUNCTIONWIDGET_H
