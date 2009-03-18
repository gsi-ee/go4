#ifndef TGO4HISDRAWOPTIONSSHORT_H
#define TGO4HISDRAWOPTIONSSHORT_H

#include "QGo4Widget.h"
#include "ui_TGo4HisDrawOptionsShort.h"  

class TGo4HisDrawOptionsShort : public QGo4Widget, public Ui::TGo4HisDrawOptionsShort
 {
     Q_OBJECT

 public:
     TGo4HisDrawOptionsShort(QWidget *parent = 0, const char* name=0);

	void ChangeColor(int kind);

	void SetDrawOpt(int kind, int value, char *dropt);

 public slots:


	virtual void SlotXlin();
	
	virtual void SlotXlog();
	
	virtual void SlotYlin();
	
	virtual void SlotYlog();
	
	virtual void SlotZlin();
	
	virtual void SlotZlog();
	
	virtual void SlotD1his();
	
	virtual void SlotD1line();
	
	virtual void SlotD2scatter();
	
	virtual void SlotD2pixel();
	
	virtual void SlotD2surfcol();
	
	virtual void SlotD2lego();
	
	virtual void SlotD2contfill();
	
	virtual void SlotD2contline();
	
	virtual void SlotLineColor();
	
	virtual void SlotFillColor();
	
	virtual void SlotMarkerColor();

 };
	
#endif	