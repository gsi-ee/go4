#ifndef TGO4ANALYSISSTATUSMONITOR_H
#define TGO4ANALYSISSTATUSMONITOR_H

#include "QGo4Widget.h"
#include "ui_TGo4AnalysisStatusMonitor.h"  

#include <QMovie>
 

class TGo4AnalysisStatusMonitor : public QGo4Widget, public Ui::TGo4AnalysisStatusMonitor
 {
     Q_OBJECT

 public:
     TGo4AnalysisStatusMonitor(QWidget *parent = 0, const char* name=0);

	 virtual ~TGo4AnalysisStatusMonitor();


	virtual void WorkWithRatemeter(TGo4Slot* slot);


	virtual void linkedObjectUpdated( const char * linkname, TObject * obj );

	virtual void linkedObjectRemoved(const char * linkname);
  
  public slots:
  
  protected:
  
	  QMovie* fxRunMovie;
	

 };
 
#endif