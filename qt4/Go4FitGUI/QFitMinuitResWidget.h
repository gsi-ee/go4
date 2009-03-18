#ifndef QFITMINUITRESWIDGET_H
#define QFITMINUITRESWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitMinuitResWidget.h"  

class TGo4FitMinuitResult;

class QFitMinuitResWidget : public QFitNamedWidget, public Ui::QFitMinuitResWidget
 {
    Q_OBJECT

public:
    QFitMinuitResWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitMinuitResult * GetResult();
    virtual void FillSpecificData();


};

#endif // QFITMINUITRESWIDGET_H








