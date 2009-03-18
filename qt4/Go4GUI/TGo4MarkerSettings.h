#ifndef TGO4MARKERSETTINGS_H
#define TGO4MARKERSETTINGS_H

#include <QDialog>
#include "ui_TGo4MarkerSettings.h"  
#include "TGo4Condition.h"
#include "TGo4Marker.h"

class TGo4MarkerSettings : public QDialog, public Ui::TGo4MarkerSettings
 {
     Q_OBJECT

 public:
     	TGo4MarkerSettings( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );	 

 public slots:

	virtual void setFlags();
 };
 
#endif