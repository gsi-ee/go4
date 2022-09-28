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

#ifndef TGO4EDITDYNENTRY_H
#define TGO4EDITDYNENTRY_H

#include "QGo4Widget.h"
#include "ui_TGo4EditDynEntry.h"

class QGo4LineEdit;
class QSpinBox;
class TObject;
class TGo4DynamicEntry;
class TGo4HistogramEntry;
class TGo4TreeHistogramEntry;

class TGo4EditDynEntry : public QGo4Widget, public Ui::TGo4EditDynEntry
{
    Q_OBJECT

public:

     enum ESelectedType {entry_None = -1, entry_Histogram = 0, entry_Tree = 1};


    TGo4EditDynEntry(QWidget *parent = nullptr, const char *name = nullptr);

    bool IsAcceptDrag(const char *itemname, TClass *cl, int kind) override;
    void DropItem(const char *itemname, TClass *cl, int kind) override;
    void linkedObjectUpdated(const char *linkname, TObject *obj) override;
    void ResetWidget() override;
    virtual QString GetEventName(const QString &itemname);
    virtual QString GetEventMember(const QString &itemname);
    virtual void WorkWithEntry(const char *itemname);
    virtual void DropHistogram(const char *itemname);
    virtual void DropCondition(const char *itemname);
    virtual void DropTree(const char *itemname);
    virtual void ProcessDropEvent(QGo4LineEdit *edt);
    virtual void ProcessTreeDropEvent(QGo4LineEdit *edt);
    void requestedObjectCreated(const char *itemname, TClass *cl) override;
    virtual bool PrepareEntryForAnalysis();
    virtual void RefreshWidget();

 public slots:

    virtual void RefreshClicked();
    virtual void ApplyClicked();
    virtual void SetPleaseUpdate( bool on );
    virtual void EntryChangedSlot();
    virtual void SaveEntrySlot();
    virtual void EvXnameEdit_dropped();
    virtual void EvYnameEdit_dropped();
    virtual void EvZnameEdit_dropped();
    virtual void ConXnameEdit_dropped();
    virtual void ConYnameEdit_dropped();
    virtual void DrawExprEdit_dropped();
    virtual void CutExprEdit_dropped();
    virtual void HisCreateButton_clicked();
    virtual void HisInfoButton_clicked();
    virtual void ConNewButton_clicked();
    virtual void ConInfoButton_clicked();
    virtual void EntryEnabledButton_toggled( bool on );
    virtual void EvXnameEdit_textChanged( const QString & xname );
    virtual void EvYnameEdit_textChanged( const QString & yname );
    virtual void EvZnameEdit_textChanged( const QString & zname );
    virtual void ConXnameEdit_textChanged( const QString & xname );
    virtual void ConYnameEdit_textChanged( const QString & yname );
    virtual void DrawExprEdit_textChanged( const QString & value );
    virtual void CutExprEdit_textChanged( const QString & value );
    virtual void DynIntervalSpin_valueChanged( int value );
    virtual void DrawButton_clicked();
    virtual void ClearHistogramClicked();
    virtual void PrintDynList();
    virtual void ConRemoveButton_clicked();

 protected:
    int fiSelectedType{0};
    bool fbTypingMode{false};

};

#endif
