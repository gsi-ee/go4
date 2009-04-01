#ifndef QFITMINUITWIDGET_H
#define QFITMINUITWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitMinuitWidget.h"

class TGo4FitMinuit;

class QFitMinuitWidget : public QFitNamedWidget, public Ui::QFitMinuitWidget
 {

    Q_OBJECT

public:
    QFitMinuitWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitMinuit * GetMinuit();
    virtual void FillSpecificData();
    virtual void CommandsEdit_textChanged();


};

#endif




