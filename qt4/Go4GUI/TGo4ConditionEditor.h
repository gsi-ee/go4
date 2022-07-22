// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
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
class TGo4ShapedCond;
class TCutG;
class TGo4ListCond;

class TGo4ConditionEditor : public QGo4Widget, public Ui::TGo4ConditionEditor
{
    Q_OBJECT

public:
    TGo4ConditionEditor(QWidget *parent = nullptr, const char* name = nullptr);

    bool IsAcceptDrag(const char *itemname, TClass *cl, int kind) override;
    void DropItem(const char *itemname, TClass *cl, int kind) override;
    void linkedObjectUpdated(const char *linkname, TObject *obj) override;
    void linkedObjectRemoved(const char *linkname) override;
    virtual void WorkWithCondition(const char *itemname);
    void ResetWidget() override;
    virtual void RefreshWidget(bool checkindex);
    virtual TGo4Condition *SelectedCondition();
    virtual bool UpdateLimits();
    virtual void ChangeConditionProperty(int id, bool on);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    void enterEvent(QEvent *) override;
#else
    void enterEvent(QEnterEvent *) override;
#endif
    void leaveEvent(QEvent *) override;
    virtual void SetChangeFlag(bool changed);
    virtual void DrawCondition(bool useactive);
    virtual void RedrawCondition();
    virtual bool PrepareForAnalysis();
    virtual void FillCutWidget(TCutG *cut);
    virtual void FillEllipseWidget(TGo4ShapedCond *elli);
    virtual void FillListWidget(TGo4ListCond *lcon);
    virtual void ShowEllipseWidget(bool show);
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

    void CutTable_contextMenuHandler(int, bool);

    bool fbTypingMode{false};
    int fiSelectedIndex{0};
    bool fbDrawOnNextRefresh{false};
    int fiLastChangeValue{0};
    bool fbEllipseAutoRefresh{false};
    bool fbNewWindow{false};

 };

#endif
