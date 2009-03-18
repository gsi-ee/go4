#ifndef QFITPEAKFINDERWIDGET_H
#define QFITPEAKFINDERWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitPeakFinderWidget.h"  

class TGo4FitPeakFinder;

class QFitPeakFinderWidget : public QFitNamedWidget, public Ui::QFitPeakFinderWidget
 {
    Q_OBJECT

public:
    QFitPeakFinderWidget( QWidget* parent = 0, const char* name = 0 );


public slots:
    virtual TGo4FitPeakFinder * GetPF();
    virtual void FillSpecificData();
    virtual void DataNameEdt_textChanged( const QString & );
    virtual void ClearModelsChk_toggled( bool );
    virtual void UsePolynChk_toggled( bool );
    virtual void PolynSpin_valueChanged( int );
    virtual void FindersTab_currentChanged( QWidget * );
    virtual void ThresholdEdt_textChanged( const QString & );
    virtual void MinWidthEdt_textChanged( const QString & );
    virtual void MaxWidthEdt_textChanged( const QString & );
    virtual void WidthEdit_textChanged( const QString & );
    virtual void NoiseFactorEdit_textChanged( const QString & );
    virtual void NoiseMinEdit_textChanged( const QString & );
    virtual void SumUpSpin_valueChanged( int );


};

#endif // QFITPEAKFINDERWIDGET_H


