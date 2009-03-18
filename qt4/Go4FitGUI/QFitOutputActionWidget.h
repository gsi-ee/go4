#ifndef QFITOUTPUTACTIONWIDGET_H
#define QFITOUTPUTACTIONWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitOutputActionWidget.h"  


class TGo4FitterOutput;


class QFitOutputActionWidget : public QFitNamedWidget, public Ui::QFitOutputActionWidget
 {

    Q_OBJECT

public:
    QFitOutputActionWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitterOutput * GetOutAct();
    virtual void FillSpecificData();
    virtual void OptionsEdit_textChanged( const QString & str );
    virtual void CommandCombo_activated( int );

};

#endif // QFITOUTPUTACTIONWIDGET_H


