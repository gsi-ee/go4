#ifndef QFITMODELFORMULAWIDGET_H
#define QFITMODELFORMULAWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelFormulaWidget.h"  


class TGo4FitModelFormula;



class QFitModelFormulaWidget : public QFitModelWidget, public Ui::QFitModelFormulaWidget
 {
    Q_OBJECT

public:
    QFitModelFormulaWidget( QWidget* parent = 0, const char* name = 0 );


public slots:
    virtual TGo4FitModelFormula * GetFormula();
    virtual void FillSpecificData();
    virtual void ExprEdt_textChanged( const QString & );
    virtual void NumParSpin_valueChanged( int );

};

#endif // QFITMODELFORMULAWIDGET_H





