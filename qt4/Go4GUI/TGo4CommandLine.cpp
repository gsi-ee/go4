// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4CommandLine.h"

#include <iostream>

#include "TROOT.h"

#include "TGo4Log.h"
#include "TGo4QSettings.h"
#include "TGo4MacroDialog.h"

#include <QFileDialog>

TGo4CommandLine::TGo4CommandLine(QWidget *parent, const char *name) :
   QGo4Widget(parent, name),fbPythonBound(false)
{
   setupUi(this);

   QObject::connect(InputLine, &QGo4CommandsHistory::enterPressedSingal, this, &TGo4CommandLine::enterPressedSlot);
   QObject::connect(ExecuteBut, &QPushButton::clicked, this, &TGo4CommandLine::ExecuteSlot);
   QObject::connect(PredefinedBut, &QPushButton::clicked, this, &TGo4CommandLine::PredefinedDialog);
   QObject::connect(FileSearchBut, &QPushButton::clicked, this, &TGo4CommandLine::FileSearchDialog);

   LoadHistory();
}


void TGo4CommandLine::FileSearchDialog()
{
   QFileDialog fd( this, "Select ROOT macro to execute in GUI", QString(),
                  "ROOT macro  (*.C *.c);;Go4 hotstart script (*.hotstart);;Python script(*.py)");
   fd.setFileMode( QFileDialog::ExistingFile);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;
   bool iscint = fd.selectedNameFilter().contains(".C") ||  fd.selectedNameFilter().contains(".c");
   bool ishot = fd.selectedNameFilter().contains(".hotstart");
   bool ispyth = fd.selectedNameFilter().contains(".py");

   QString cmd;
   if (iscint) {
      cmd = QString(".x ") + flst[0];
   } else if (ispyth) {
      cmd = QString("$") + flst[0];
   } else if (ishot) {
      cmd = flst[0];
   } else {
      std::cout << "TGo4CommandLine NEVER COME HERE - unknown file type!\n-" << std::endl;
      return;
   }

   int index = InputLine->findText(cmd);
   if (index < 0) {
      InputLine->insertItem(-1, cmd);
      index = InputLine->findText(cmd);
   }
   InputLine->setCurrentIndex(index);
}

void TGo4CommandLine::ExecuteSlot()
{
   enterPressedSlot();
}

void TGo4CommandLine::enterPressedSlot()
{
  const char pyprompt = '$';
  QString pyinitfile="python/go4init.py"; // JAM put this to settings later?
  QString str = InputLine->currentText();
  if (str.isEmpty())
    return;

  if (str.contains("help") || str.contains(".go4h"))
  {
    PrintHelp();
  }
  else if (str.contains(".hotstart") && !str.contains(".x"))
  {
    StatusMessage(QString("Executing hotstart script: ") + str);
    StartHotstart(str.toLatin1().constData());
  }
  else
  {
    // process cint line, check if python or not:
    if (str.contains(pyprompt))
    {
      // Python support as initiated by Sven Augustin, MPI Heidelberg
      str.remove(0, str.indexOf(pyprompt) + 1);
      StatusMessage(QString("Executing Python script: ") + str);
      const QString PY_EXT = ".py";
      int imin = str.indexOf(PY_EXT + " ");
      int imax = str.length();

      if (imin == -1) {
        imin = imax;
      } else {
        imin += PY_EXT.length();
      }

      QString scrstring = str.left(imin);
      QString argstring = str.mid(imin);

      str  = "TPython::Exec(\"import sys; sys.argv = [\'" + scrstring + "\'] + \'" + argstring + "\'.split()\");";
      str += "TPython::LoadMacro(\"" + scrstring + "\")";
      if (!fbPythonBound)
      {
        QString go4sys=TGo4Log::GO4SYS();
        str.prepend("TPython::LoadMacro(\"" + go4sys + pyinitfile +"\");");
        str.prepend("TPython::Bind(TGo4AbstractInterface::Instance(), \"go4\");");
        fbPythonBound = kTRUE;
      }

    }
    else
    {
      StatusMessage(QString("Executing command: ") + str);
    }
    try
    {
      gROOT->ProcessLineSync(str.toLatin1().constData());
      go4sett->setCommandsHistoryGUI(InputLine->getHistory(50));
    }
    catch(std::exception& ex)
    {
      StatusMessage(QString("Error invoking command - Exception: ") + QString(ex.what()));
    }
    catch(...)
    {
      StatusMessage(QString("Error invoking command - unknown Exception!"));
    }

  }

}

void TGo4CommandLine::LoadHistory()
{
    // read command history from settings:
    QStringList histlist = go4sett->getCommandsHistoryGUI();
    InputLine->addItems(histlist);
    // prepared pre-loading of system macros:

    auto load_macro = [](const char *name) {
       TString exec = ".L ";
       exec += TGo4Log::subGO4SYS(name);
       gROOT->ProcessLineSync(exec.Data());
    };

    load_macro("macros/corrhistos.C");
    load_macro("macros/hishisto.C");
    load_macro("macros/addhistos.C");
    load_macro("macros/divhistos.C");
    load_macro("macros/profileX.C");
    load_macro("macros/profileY.C");
    load_macro("macros/projectionX.C");
    load_macro("macros/projectionY.C");
    load_macro("macros/rebin.C");
    load_macro("macros/scalex.C");
    load_macro("macros/fft.C");
}


void TGo4CommandLine::PredefinedDialog()
{
   TGo4MacroDialog md;
   if (md.exec() != QDialog::Accepted) return;
   InputLine->insertItem(0, md.getCommand());
   InputLine->setCurrentIndex(0);
}


void TGo4CommandLine::PrintHelp()
{
   std::cout <<"\n--- Go4 GUI command line short help --- " << std::endl;
   std::cout <<"- execute any command by pressing RETURN (Enter key)" << std::endl;
   std::cout <<"--" << std::endl;
   std::cout <<"- use '.h' for help concerning ROOT commands" << std::endl;
   std::cout <<"--" << std::endl;
   std::cout <<"- use 'go4->...' to access TGo4AbstractInterface for gui commands" << std::endl;
   std::cout <<"--" << std::endl;
   std::cout <<"- Some useful Go4 GUI command functions:" << std::endl;

   std::cout <<"\t- TObject *go4->GetObject(const char *itemname);"<< std::endl;
   std::cout <<"\t\t: get browser object by full pathname. " << std::endl;
   std::cout <<"\t- TString go4->FindItem(const char *name); " << std::endl;
   std::cout <<"\t\t: get full pathname of browser object by name " << std::endl;
   std::cout <<"\t- TString go4->SaveToMemory(const char *path, TObject *obj, Bool_t ownership = kFALSE);"<< std::endl;
   std::cout <<"\t\t: Put object obj into workspace memory under subfolder path. Returns full item pathname. " << std::endl;
   std::cout <<"\t- ViewPanelHandle go4->StartViewPanel()"<< std::endl;
   std::cout <<"\t\t: Open new Go4 viewpanel with default size. Returns handle of new panel." << std::endl;
   std::cout <<"\t- Bool_t go4->DrawItem(const char *itemname, ViewPanelHandle panel = nullptr, const char *drawopt = nullptr);"<< std::endl;
   std::cout <<"\t\t: Draw object of full name itemname onto viewpanel panel. Will open new viewpanel if panel not specified. \n\t  ROOT drawoption may be set. " << std::endl;
   std::cout <<"\t- go4->OpenFile(const char *fname)" << std::endl;
   std::cout <<"\t\t: open ROOT file of fname in Go4 browser " << std::endl;
   std::cout <<"\t- go4->LaunchAnalysis()" << std::endl;
   std::cout <<"\t\t: Start Go4 analysis process with previous set up " << std::endl;
   std::cout <<"--" << std::endl;
   std::cout <<"---- Please: note the Go4 helpwindow with TGo4AbstractInterface full method documentation!" << std::endl;
   std::cout <<"----         see example scripts at $GO4SYS/macros !" << std::endl;
   std::cout <<"----         visit our website at: \t http://go4.gsi.de !" << std::endl;
   std::cout <<"------ " << std::endl;
   HelpWindow("docs/Go4Reference.pdf", "Show Go4 Reference manual...");
}
