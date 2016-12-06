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


#include <iostream>

TGo4UserCommands::TGo4UserCommands( QWidget* parent, const char* name, Qt::WindowFlags fl ) :
   QWidget( parent, fl )
{

	fMainWindow=dynamic_cast<TGo4MainWindow*> (parent);


   setObjectName( name ? name : "Go4UserCommands");
   setupUi(this);

   // TODO: here connect signals
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

   fAnalysisMacroMonitorCheck.clear();
   fAnalysisMacroMonitorCheck.push_back(MonitorBox1);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox2);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox3);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox4);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox5);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox6);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox7);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox8);
   fAnalysisMacroMonitorCheck.push_back(MonitorBox9);

   for (int i = 0; i < GO4GUI_MAXMACRONUM; ++i)
    {
	   fAnalysisMacroButtons[i]->setEnabled(false);
	   fAnalysisMacroMonitorCheck[i]->setEnabled(false);
      int butid = i + 1;
      fAnalysisMacroButtons[i]->setShortcut(QString("Ctrl+%1").arg(butid));
      QString com = go4sett->getAnalysisMacroCommand(i);
      if (com.isEmpty())
      {
        fAnalysisMacroCommands.push_back(QString(""));
        continue;
      }
      fAnalysisMacroButtons[i]->setEnabled(true);
      fAnalysisMacroButtons[i]->setToolTip(go4sett->getAnalysisMacroTip(i));

      fAnalysisMacroCommands.push_back(com);

      fAnalysisMacroMonitorCheck[i]->setEnabled(true);
      fAnalysisMacroMonitorCheck[i]->setToolTip(QString("Enable Timer Execution for command %1").arg(i+1));

    }


    QObject::connect(CommandAutoButton, SIGNAL (clicked ()), this, SLOT (AnalysisMacroMonitorBtn_clicked ()));

    fAnalysisMacroTimer = new QTimer(this);
    QObject::connect(fAnalysisMacroTimer, SIGNAL (timeout ()), this, SLOT (AnalysisMacroMonitorTimeout()));

    QObject::connect(ConfigureButton, SIGNAL (clicked ()), this, SLOT (ConfigureAnalysisMacros()));

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
   for(int i=0; i<fAnalysisMacroMonitorCheck.size(); ++i)
    {
       if(fAnalysisMacroMonitorCheck[i]->isChecked())
           ExecuteAnalysisMacro(i);
    }

}


void TGo4UserCommands::ConfigureAnalysisMacros()
{
  //std::cout << "ConfigureAnalysisMacros " <<std::endl;

  // first we just reproduce what we had. later use configuration dialog here!
  if(!ConfigureButton->isChecked())
    {
      //std:cout << "fAnalysisMacroConfigure is not checked"<< std::endl;
      // here we must enable only the buttons with valid configuration:
      for(int i=0; i<GO4GUI_MAXMACRONUM;++i)
          {
            QString com=go4sett->getAnalysisMacroCommand(i);
            if(com.isEmpty()){
                fAnalysisMacroButtons[i]->setEnabled(false);
                fAnalysisMacroMonitorCheck[i]->setEnabled(false);
            }
          }
      return;
    }
  // we try some interactive game:
  //1) first popup of info window


  QMessageBox::information(this,"Configuration of Analysis Macros", "Please click on the Command button that should be configured and define command or macro action for analysis process.");
  for(int i=0; i<GO4GUI_MAXMACRONUM;++i)
     {
	  fAnalysisMacroButtons[i]->setEnabled(true); // need to activate all buttons in this mode!
      fAnalysisMacroMonitorCheck[i]->setEnabled(true);

     }

// the rest is handled in the button slots themselves!

  //2) user must click on function button to configure
  //3) popup of requester for command and tooltip
  //4) done

  // second click on configure button must end this mode! <- automatically by action toggle!





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
  if(ConfigureButton->isChecked())
    {
      DefineAnalysisMacro(id);
      ConfigureButton->setChecked(false);
      ConfigureAnalysisMacros(); // set non defined buttons disabled again, this slot is not called automatically with setChecked!
      return;
    }
  QString cmd=fAnalysisMacroCommands[id];
  if (cmd.length()==0) return;
  //std::cout << "Invoke custom analysis command: "<< cmd.toLatin1().constData() <<std::endl;
  fMainWindow->StatusMessage(QString("Invoke custom analysis command: ").append(cmd));
  TGo4ServerProxy* serv = fMainWindow->Browser()->FindServer();
  if (serv!=0)
  {
    // TODO: have to check priviliges here?
    serv->ExecuteLine(cmd.toLatin1().constData());
  }
}


void TGo4UserCommands::DefineAnalysisMacro(int id)
{
  bool ok;
  int butid=id+1;
  QString title=QString("Analysis command button %1 definition").arg(butid);
  QString message=QString("Please specify analysis interpreter command to execute on button C%1 : <br>'@' means 'TGo4Analysis::Instance()->'<br>'.x' will invoke ROOT script<br>'$' will invoke Python.").arg(butid);
  QString com=QInputDialog::getText(this, title, message, QLineEdit::Normal, go4sett->getAnalysisMacroCommand(id), &ok);
  //if (!ok || com.isEmpty()) return;
  if (!ok) return;
  message=QString("Please specify tool tip for button C%1").arg(butid);
  QString deftip=go4sett->getAnalysisMacroTip(id).split(":").first();
  QString tip=QInputDialog::getText(this, title, message, QLineEdit::Normal, deftip, &ok);
  if (!ok) return;
  go4sett->setAnalysisMacroCommand(id, com);
  tip.append(": ");
  tip.append(fAnalysisMacroButtons[id]->shortcut().toString());
  go4sett->setAnalysisMacroTip(id, tip);
  fAnalysisMacroCommands[id]=com;
  fAnalysisMacroButtons[id]->setToolTip(tip);
}





void TGo4UserCommands::ClearAnalysisMacros()
{
  if(QMessageBox::question( this, "Analysis Macro Buttons", "Really Clear all user defined macro commands?",
          QMessageBox::Yes | QMessageBox::No ,
          QMessageBox::Yes) != QMessageBox::Yes ) {
             return;
       }


  for(int i=0; i<GO4GUI_MAXMACRONUM;++i)
      {
	  	 fAnalysisMacroButtons[i]->setEnabled(false);
        fAnalysisMacroCommands[i]="";
        go4sett->setAnalysisMacroCommand(i,"");
        go4sett->setAnalysisMacroTip(i,"");
      }
}


