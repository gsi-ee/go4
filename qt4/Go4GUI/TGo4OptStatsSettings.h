#ifndef TGO4OPTSTATSSETTINGS_H
#define TGO4OPTSTATSSETTINGS_H

#include <QDialog>
#include "ui_TGo4OptStatsSettings.h"

class TGo4OptStatsSettings : public QDialog, public Ui::TGo4OptStatsSettings
 {
     Q_OBJECT

 public:
	TGo4OptStatsSettings( QWidget* parent = 0 );

 public slots:

	void setFlags();

 };

#endif
