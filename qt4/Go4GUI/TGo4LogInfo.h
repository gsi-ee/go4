#ifndef TGO4LOGINFO_H
#define TGO4LOGINFO_H

#include "QGo4Widget.h"
#include "ui_TGo4LogInfo.h"  

#include <QPixmap>



class TGo4LogInfo : public QGo4Widget, public Ui::TGo4LogInfo
 {
     Q_OBJECT

 public:
     TGo4LogInfo(QWidget *parent = 0, const char* name=0);
	
	void WorkWithInfo(TGo4Slot* slot);
	
	void linkedObjectUpdated( const char * linkname, TObject * obj );
	
 public slots:
	
	virtual void ClearLogInfo();
	
	virtual void SaveLogInfo();
	
 protected:
    QPixmap infoIcon;
    QPixmap warnIcon;
    QPixmap errorIcon;
	
 };
 
#endif