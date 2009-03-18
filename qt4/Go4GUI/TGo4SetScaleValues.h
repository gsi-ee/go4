#ifndef TGO4SETSCALEVALUES_H
#define TGO4SETSCALEVALUES_H

#include <QWidget>
#include "ui_TGo4SetScaleValues.h"  
class TGo4ViewPanel;
class TPad;

class TGo4SetScaleValues : public QWidget, public Ui::TGo4SetScaleValues
 {
     Q_OBJECT

 public:
	 TGo4SetScaleValues( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );

	 void refreshView(bool force);

 public slots:

	virtual void panelSlot(TGo4ViewPanel* panel, TPad* pad, int signalid);
	
	virtual void ApplyValues();
	
	virtual void AutoscaleChanged(bool on);

protected:
    bool fbDoingRefresh;

    virtual void closeEvent( QCloseEvent * ce );


 };
 
#endif