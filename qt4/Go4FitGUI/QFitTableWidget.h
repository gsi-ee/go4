#ifndef QFITTABLEWIDGET_H
#define QFITTABLEWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitTableWidget.h"  



class QFitTableWidget : public QFitWidget, public Ui::QFitTableWidget
 {
    Q_OBJECT

public:
    QFitTableWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual void FillSpecificData();

};

#endif // QFITTABLEWIDGET_H


