#ifndef TGO4CONDITIONINFO_H
#define TGO4CONDITIONINFO_H


#include "QGo4Widget.h"
#include "ui_TGo4ConditionInfo.h"  

class TGo4Condition;





class TGo4ConditionInfo : public QGo4Widget, public Ui::TGo4ConditionInfo
 {
     Q_OBJECT

 public:
     
 enum PropertyIndex {
   CONTITLE,
   CONCLASS,
   CONDIM,
   CONX,
   CONY,
   CONCOUNTS,
   CONRESULT,
   CONMODE,
   CONSIZE,
   CONTIME
};

	 
	 
	 
	 TGo4ConditionInfo(QWidget *parent = 0, const char* name=0);

 public:
 
 	virtual bool IsAcceptDrag(const char* itemname, TClass* cl, int kind);
	
	virtual void DropItem(const char* itemname, TClass* cl, int kind);
	
	virtual void linkedObjectUpdated(const char* linkname, TObject* obj);

	virtual void WorkWithCondition(const char* itemname);
	
	virtual void ResetWidget();
	
	virtual void RefreshWidget(TGo4Condition* cond);

 public slots:

		
	virtual void ConInfoButton_clicked();
	
	virtual void ConPrintButton_clicked();
	
	virtual void ConEditButton_clicked();
	
	virtual void PrintLog_clicked();
	
	

};


#endif