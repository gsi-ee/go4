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

 
     TGo4EditDynEntry(QWidget *parent = 0, const char* name = 0);

	virtual bool IsAcceptDrag( const char * itemname, TClass * cl, int kind );
    virtual void DropItem( const char * itemname, TClass * cl, int kind );
    virtual void linkedObjectUpdated( const char * linkname, TObject * obj );
    virtual void RefreshWidget();
    virtual QString GetEventName( const QString & itemname );
    virtual QString GetEventMember( const QString & itemname );
    virtual void WorkWithEntry( const char * itemname );
    virtual void DropHistogram( const char * itemname );
    virtual void DropCondition( const char * itemname );
    virtual void DropTree( const char * itemname );
    virtual void ResetWidget();
    virtual void ProcessDropEvent( QGo4LineEdit * edt );
    virtual void ProcessTreeDropEvent( QGo4LineEdit * edt );
    virtual void requestedObjectCreated( const char * itemname, TClass * cl );
    virtual bool PrepareEntryForAnalysis();


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
    int fiSelectedType;
    bool fbTypingMode;


};

#endif