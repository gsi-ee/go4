// $Id: TGo4UserCommands.cpp 1134 2014-01-22 14:53:40Z linev $
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

#include "TGo4UserCommands.h"
#include "TGo4QSettings.h"
#include "TGo4MainWindow.h"
#include "TGo4BrowserProxy.h"
#include "TGo4ServerProxy.h"

#include <QMessageBox>
#include <QInputDialog>

#include "TGo4UserCommandsDialog.h"

#include <iostream>

TGo4UserCommands::TGo4UserCommands( QWidget* parent, const char* name, Qt::WindowFlags fl ) :
   QGo4Widget( parent, name , fl)
{
   setObjectName( name ? name : "Go4UserCommands");
   setupUi(this);

   QObject::connect (CommandButton1, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_1 ()));
   QObject::connect (CommandButton2, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_2 ()));
   QObject::connect (CommandButton3, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_3 ()));
   QObject::connect (CommandButton4, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_4 ()));
   QObject::connect (CommandButton5, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_5 ()));
   QObject::connect (CommandButton6, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_6 ()));
   QObject::connect (CommandButton7, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_7 ()));
   QObject::connect (CommandButton8, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_8 ()));
   QObject::connect (CommandButton9, SIGNAL (pressed ()), this, SLOT (ExecuteAnalysisMacro_9 ()));

   // here assign widget arrays:
   fAnalysisMacroButtons.clear();
   fAnalysisMacroButtons.push_back(CommandButton1);
   fAnalysisMacroButtons.push_back(CommandButton2);
   fAnalysisMacroButtons.push_back(CommandButton3);
   fAnalysisMacroButtons.push_back(CommandButton4);
   fAnalysisMacroButtons.push_back(CommandButton5);
   fAnalysisMacroButtons.push_back(CommandButton6);
   fAnalysisMacroButtons.push_back(CommandButton7);
   fAnalysisMacroButtons.push_back(CommandButton8);
   fAnalysisMacroButtons.push_back(CommandButton9);


   for (int i = 0; i < GO4GUI_MAXMACRONUM; ++i)
    {
	   fAnalysisMacroAutoExecute.push_back(false);
	   fAnalysisMacroButtons[i]->setEnabled(false);

      QString com = go4sett->getAnalysisMacroCommand(i);
      if (com.isEmpty())
      {
        fAnalysisMacroCommands.push_back(QString(""));
        continue;
      }
      fAnalysisMacroButtons[i]->setEnabled(true);
      fAnalysisMacroButtons[i]->setToolTip(go4sett->getAnalysisMacroTip(i));
      fAnalysisMacroCommands.push_back(com);
      SetAutoExecute(i,go4sett->getAnalysisMacroAutomode(i));
    }


    QObject::connect(CommandAutoButton, SIGNAL (clicked ()), this, SLOT (AnalysisMacroMonitorBtn_clicked ()));

    fAnalysisMacroTimer = new QTimer(this);
    QObject::connect(fAnalysisMacroTimer, SIGNAL (timeout ()), this, SLOT (AnalysisMacroMonitorTimeout()));

    QObject::connect(ConfigureButton, SIGNAL (clicked ()), this, SLOT (ConfigureAnalysisMacros()));
#ifdef 	USERCOMMANDS_CONFIGWINDOW
    ConfigureButton->setCheckable(false);
#endif

    AssignShortcuts();
}

void TGo4UserCommands::AssignShortcuts()
{
	// try to redefine shortcuts here after this widget was adopted by mainwindow toolbar
	// seems that Qt5 overrides initial definitions:
	for (int i = 0; i < GO4GUI_MAXMACRONUM; ++i)
	    {
		 	 int butid = i + 1;
		 	 fAnalysisMacroButtons[i]->setShortcut(QString("Ctrl+%1").arg(butid));
//		 	 std::cout << "set button shortcut "<<i<<" to "<<fAnalysisMacroButtons[i]->shortcut().toString().toLatin1 ().constData ()<< std::endl;
		  	 //fAnalysisMacroButtons[i]->setShortcut(QKeySequence(Qt::CTRL,  Qt::Key_0 + butid));
	    }

}

void TGo4UserCommands::AnalysisMacroMonitorBtn_clicked()
{
  //std::cout << "AnalysisMacroMonitorBtn_clicked " <<std::endl;

  if(fAnalysisMacroTimer->isActive())
  {
    fAnalysisMacroTimer->stop();
    CommandAutoButton->setIcon(QIcon( ":/icons/startselected.png" ));
    CommandAutoButton->setToolTip("Start Command Execution Timer");
    AutoTimeSpinBox->setEnabled(true);
  }
  else
  {
    double t=1000.0*AutoTimeSpinBox->value();
    fAnalysisMacroTimer->start(t);
    CommandAutoButton->setIcon(QIcon( ":/icons/Stop.png" ));
    CommandAutoButton->setToolTip("Stop Command Execution Timer");
    AutoTimeSpinBox->setEnabled(false);

  }
}


void TGo4UserCommands::AnalysisMacroMonitorTimeout()
{
  //std::cout << "AnalysisMacroMonitorTimeout" <<std::endl;
  if(ConfigureButton->isChecked())
  {
      AnalysisMacroMonitorBtn_clicked(); // stop timer in editor mode
      return;
  }
   for(int i=0; i<fAnalysisMacroAutoExecute.size(); ++i)
    {
       if(fAnalysisMacroAutoExecute[i])
    	   ExecuteAnalysisMacro(i);
    }

}


void TGo4UserCommands::ConfigureAnalysisMacros()
{
  //std::cout << "ConfigureAnalysisMacros " <<std::endl;

TGo4UserCommandsDialog setup(this);
if ( setup.exec() != QDialog::Accepted ) return;

//AssignShortcuts(); // test if we can manipulate them afterwards.

for(int id=0; id<GO4GUI_MAXMACRONUM;++id)
{
	// get from requester and store in go4 settings:
	QString com=setup.GetCommand(id);
	go4sett->setAnalysisMacroCommand(id, com);
	QString tip=setup.GetTooltip(id);
	tip.append(": ");
	tip.append(fAnalysisMacroButtons[id]->shortcut().toString());
	go4sett->setAnalysisMacroTip(id, tip);
	//std::cout<<"ConfigureAnalysisMacros - index"<<id<<" set command "<<com.toLatin1 ().constData ()<<", tip:"<<tip.toLatin1 ().constData ()<< std::endl;

	bool execute=setup.GetAutoExecute(id);
	go4sett->setAnalysisMacroAutomode(id, execute);

	// put new setup to gui:
	fAnalysisMacroCommands[id]=com;
	fAnalysisMacroButtons[id]->setToolTip(tip);

	SetAutoExecute(id,execute);


	// now activate only such elements that are defined:
	fAnalysisMacroButtons[id]->setEnabled(true);
	//fAnalysisMacroMonitorCheck[id]->setEnabled(true);
	 if(com.isEmpty()){
		 fAnalysisMacroButtons[id]->setEnabled(false);
		 //fAnalysisMacroMonitorCheck[id]->setEnabled(false);
	 }


}




}

void TGo4UserCommands::ExecuteAnalysisMacro_1()
{
  ExecuteAnalysisMacro(0); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_2()
{
  ExecuteAnalysisMacro(1); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_3()
{
  ExecuteAnalysisMacro(2); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_4()
{
  ExecuteAnalysisMacro(3); // note the shift of index here. want to start with ctrl-1 as first macro...
}


void TGo4UserCommands::ExecuteAnalysisMacro_5()
{
  ExecuteAnalysisMacro(4); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_6()
{
  ExecuteAnalysisMacro(5); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_7()
{
  ExecuteAnalysisMacro(6); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_8()
{
  ExecuteAnalysisMacro(7); // note the shift of index here. want to start with ctrl-1 as first macro...
}

void TGo4UserCommands::ExecuteAnalysisMacro_9()
{
  ExecuteAnalysisMacro(8); // note the shift of index here. want to start with ctrl-1 as first macro...
}



void TGo4UserCommands::ExecuteAnalysisMacro(int id)
{
  //std::cout << "ExecuteAnalysisMacro "<< id <<std::endl;
  QString cmd=fAnalysisMacroCommands[id];
  if (cmd.length()==0) return;
  //std::cout << "Invoke custom analysis command: "<< cmd.toLatin1().constData() <<std::endl;
  StatusMessage(QString("Invoke custom analysis command: ").append(cmd));
  TGo4ServerProxy* serv = Browser()->FindServer();
  if (serv!=0)
  {
    // TODO: have to check priviliges here?
    serv->ExecuteLine(cmd.toLatin1().constData());
  }
}




void TGo4UserCommands::SetAutoExecute(int id, bool on)
{
	fAnalysisMacroAutoExecute[id]=on;
	QString iconname;
	if(on)
	{
		iconname=QString(":/icons/Number-%1-icon-green.png").arg(id+1);
	}
	else
	{
		iconname=QString(":/icons/Number-%1-icon.png").arg(id+1);
	}
	fAnalysisMacroButtons[id]->setIcon(QIcon(iconname));
}




