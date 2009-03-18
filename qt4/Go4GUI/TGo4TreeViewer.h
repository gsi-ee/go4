#ifndef TGO4TREEVIEWER_H
#define TGO4TREEVIEWER_H

#include "QGo4Widget.h"
#include "ui_TGo4TreeViewer.h"  

class TObject;

class TGo4TreeViewer : public QGo4Widget, public Ui::TGo4TreeViewer
 {
     Q_OBJECT

 public:

	 TGo4TreeViewer(QWidget *parent = 0, const char* name=0);

  	virtual void ResetWidget();
    virtual void WorkWithLeaf(const char* itemname);
    virtual void ProcessDropEvent(QGo4LineEdit* edt, bool caninit);
    virtual void requestedObjectCreated(const char* itemname, TClass* cl);


 public slots:

	virtual void NewHistBtn_clicked();
    virtual void TreeClearBtn_clicked();
    virtual void TreeDrawBtn_clicked();
    virtual void XFieldEdt_dropped();
    virtual void YFieldEdt_dropped();
    virtual void ZFieldEdt_dropped();
    virtual void cutEdit_dropped();

protected:

	QString fxTreeName;



 };

#endif