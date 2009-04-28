#ifndef TGO4CREATENEWDYNENTRY_H
#define TGO4CREATENEWDYNENTRY_H


#include <QDialog>
#include "ui_TGo4CreateNewDynEntry.h"

class TGo4DynamicEntry;

class TGo4CreateNewDynEntry : public QDialog, public Ui::TGo4CreateNewDynEntry
 {
     Q_OBJECT

 public:
 	 TGo4CreateNewDynEntry( QWidget* parent = 0 );
	 TGo4DynamicEntry* MakeEntry();
 };

#endif
