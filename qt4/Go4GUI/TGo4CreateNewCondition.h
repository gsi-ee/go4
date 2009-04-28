#ifndef TGO4CREATENEWCONDITION_H
#define TGO4CREATENEWCONDITION_H


#include <QDialog>
#include "ui_TGo4CreateNewCondition.h"

class TGo4Condition;

class TGo4CreateNewCondition : public QDialog, public Ui::TGo4CreateNewCondition
 {
     Q_OBJECT

 public:
		 TGo4CreateNewCondition(QWidget* parent = 0);

		 TGo4Condition* MakeCondition();
 };

#endif
