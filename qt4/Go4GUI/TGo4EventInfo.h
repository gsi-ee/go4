#ifndef TGO4EVENTINFO_H
#define TGO4EVENTINFO_H

#include "QGo4Widget.h"
#include "ui_TGo4EventInfo.h"  

class TGo4EventInfo : public QGo4Widget, public Ui::TGo4EventInfo
 {
     Q_OBJECT

 public:
     TGo4EventInfo(QWidget *parent = 0, const char* name = 0);

	bool IsAcceptDrag(const char* itemname, TClass* cl, int kind);
	
	void DropItem(const char* itemname, TClass* cl, int kind);
	
	void WorkWithEvent(const char* itemname);
	
	void ResetWidget();
	
	void linkedObjectUpdated(const char* linkname, TObject* obj);
	
	void linkedObjectRemoved(const char* linkname);



 public slots:

	virtual void RefreshClicked();
	
	virtual void PrintEventClicked();
	
	virtual void MbsButton_clicked();



};

#endif