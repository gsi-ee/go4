#ifndef QFITPRINTWIDGET_H
#define QFITPRINTWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitPrintWidget.h"  

class QFitPrintWidget : public QFitWidget, public Ui::QFitPrintWidget
 {

    Q_OBJECT

public:
    QFitPrintWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual void FillSpecificData();
    virtual void SetDrawOption( const QString & option );

protected:
    QString fxDrawOption;


};

#endif // QFITPRINTWIDGET_H


