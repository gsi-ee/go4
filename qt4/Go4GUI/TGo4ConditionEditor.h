// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4CONDITIONEDITOR_H
#define TGO4CONDITIONEDITOR_H

#include "QGo4Widget.h"
#include "ui_TGo4ConditionEditor.h"

class TGo4Condition;
class TGo4EllipseCond;
class TCutG;


class TGo4ConditionEditor : public QGo4Widget, public Ui::TGo4ConditionEditor
 {
     Q_OBJECT

 public:
     TGo4ConditionEditor(QWidget *parent = 0, const char* name=0);

   virtual bool IsAcceptDrag( const char * itemname, TClass * cl, int kind );
    virtual void DropItem( const char * itemname, TClass * cl, int kind );
    virtual void linkedObjectUpdated( const char * linkname, TObject * obj );
    virtual void linkedObjectRemoved( const char * linkname );
    virtual void WorkWithCondition( const char * itemname );
    virtual void ResetWidget();
    virtual void RefreshWidget( bool checkindex );
    virtual TGo4Condition * SelectedCondition();
    virtual bool UpdateLimits();
    virtual void ChangeConditionProperty( int id, bool on );
    virtual void enterEvent( QEvent * );
    virtual void leaveEvent( QEvent * );
    virtual void SetChangeFlag( bool changed );
    virtual void DrawCondition( bool useactive );
    virtual void RedrawCondition();
    virtual bool PrepareForAnalysis();
    virtual void FillCutWidget( TCutG * cut );
    virtual void FillEllipseWidget(TGo4EllipseCond* elli);
    virtual void UpdateEllipse();

 public slots:

    virtual void PleaseUpdateSlot();
    virtual void SelectedCond( int t );
    virtual void ArrayAll();
    virtual void SetResultMode( int mode );
    virtual void SetInvertMode( int mode );
    virtual void ClearCounters();
    virtual void LimitsChanged( const QString & );
    virtual void LimitsReturnPressed();
    virtual void SetLabel( bool on );
    virtual void SetDrawLimits( bool on );
    virtual void SetIntDraw( bool on );
    virtual void SetXMeanDraw( bool on );
    virtual void SetXRMSDraw( bool on );
    virtual void SetYRMSDraw( bool on );
    virtual void SetYMeanDraw( bool on );
    virtual void SetXMaxDraw( bool on );
    virtual void SetYMaxDraw( bool on );
    virtual void SetCMaxDraw( bool on );
    virtual void SetCondVisible( bool on );
    virtual void RefreshClicked();
    virtual void DisplayPressed();
    virtual void UpdatePressed();
    virtual void SaveCondition();
    virtual void PrintConditionLog();
    virtual void ModifyButton_clicked();
    virtual void NPointsSpin_valueChanged( int npoint );
    virtual void CutTable_valueChanged( int ncol, int nrow );
    virtual void CutTable_contextMenuRequested(const QPoint &);
    virtual void ContextMenuSlot(int);
    virtual void EllipseTheta_returnPressed();
    virtual void EllipseNPoints_valueChanged( int npoint );
    virtual void EllipseTheta_valueChanged(int deg);
    virtual void EllipseCx_valueChanged(double x);
    virtual void EllipseCy_valueChanged(double y);
    virtual void EllipseA1_valueChanged(double r1);
    virtual void EllipseA2_valueChanged(double r2);
    virtual void EllipseRefreshBox_toggled(bool on);
    virtual void EllipseCircleBox_toggled(bool on);

protected:
    bool fbTypingMode;
    int fiSelectedIndex;
    bool fbDrawOnNextRefresh;
    int fiLastChangeValue;

    bool fbEllipseAutoRefresh;

 };

#endif
