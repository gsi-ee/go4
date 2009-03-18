#ifndef QFITDEPENDENCYWIDGET_H
#define QFITDEPENDENCYWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitDependencyWidget.h"  

class TGo4FitDependency;

class QFitDependencyWidget : public QFitWidget, public Ui::QFitDependencyWidget
 {
     Q_OBJECT


public:
    QFitDependencyWidget( QWidget* parent = 0, const char* name = 0 );
 
public slots:
    virtual TGo4FitDependency * GetDepen();
    virtual void FillSpecificData();
    virtual void ParamEdit_textChanged( const QString & );
    virtual void ExpressionEdt_textChanged( const QString & );

};

#endif


