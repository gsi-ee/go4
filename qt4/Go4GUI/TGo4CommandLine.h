#ifndef TGO4COMMANDLINE_H
#define TGO4COMMANDLINE_H

#include "QGo4Widget.h"
#include "ui_TGo4CommandLine.h"  

#include "QGo4CommandsHistory.h"


class TGo4CommandLine : public QGo4Widget, public Ui::TGo4CommandLine
{
   Q_OBJECT

   public:
      TGo4CommandLine(QWidget *parent = 0, const char* name=0);

   public Q_SLOTS:

      virtual void FileSearchDialog();

      virtual void enterPressedSlot();

      virtual void LoadHistory();

      virtual void PredefinedDialog();

      virtual void PrintHelp();
	
};
 
#endif
