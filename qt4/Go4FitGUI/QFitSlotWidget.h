#ifndef QFITSLOTWIDGET_H
#define QFITSLOTWIDGET_H

class TGo4FitSlot;

#include "QFitNamedWidget.h"
#include "ui_QFitSlotWidget.h"  

class QFitSlotWidget : public QFitNamedWidget, public Ui::QFitSlotWidget
 {

public:
    QFitSlotWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitSlot * GetSlot();
    virtual void FillSpecificData();
    virtual void RequirementChk_toggled( bool req );
    virtual void SaveCmb_activated( int typ );


};

#endif // QFITSLOTWIDGET_H


