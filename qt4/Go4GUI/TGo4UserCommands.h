//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4USERCOMMANDS_H
#define TGO4USERCOMMANDS_H


#include "QGo4Widget.h"
#include <QString>
#include <QToolButton>
#include <QCheckBox>
#include <QTimer>

#define GO4GUI_MAXMACRONUM 9



#include "ui_TGo4UserCommands.h"
#include <vector>

class TGo4UserCommands : public QGo4Widget, public Ui::TGo4UserCommands
 {
     Q_OBJECT

protected:

	 /** array to Qt designer buttons*/
	 std::vector<QToolButton*> fAnalysisMacroButtons;

	 /** command strings to execute*/
	 std::vector<QString>    fAnalysisMacroCommands;

	 /** here we remember if we want automatic timer execution */
	 std::vector<bool> fAnalysisMacroAutoExecute;



	 QTimer* fAnalysisMacroTimer;



 public:
    TGo4UserCommands( QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0 );

    void AssignShortcuts();

 public slots:


     void ConfigureAnalysisMacros();



      // default slots for macro buttons:

      void ExecuteAnalysisMacro_1();
      void ExecuteAnalysisMacro_2();
      void ExecuteAnalysisMacro_3();
      void ExecuteAnalysisMacro_4();
      void ExecuteAnalysisMacro_5();
      void ExecuteAnalysisMacro_6();
      void ExecuteAnalysisMacro_7();
      void ExecuteAnalysisMacro_8();
      void ExecuteAnalysisMacro_9();


      void ExecuteAnalysisMacro(int id);
      //void DefineAnalysisMacro(int id);

      /** activate auto execution state for command of id*/
      void SetAutoExecute(int id, bool on);



      void AnalysisMacroMonitorBtn_clicked();
      void AnalysisMacroMonitorTimeout();





};

#endif
