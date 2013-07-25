// $Id$
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

#include "TGo4CommandLine.h"

#include "Riostream.h"
#include "TROOT.h"

#include <QFileDialog>

#include "TGo4Log.h"
#include "TGo4QSettings.h"
#include "TGo4MacroDialog.h"

TGo4CommandLine::TGo4CommandLine(QWidget *parent, const char* name) :
   QGo4Widget(parent, name)
{
   setupUi(this);
   LoadHistory();
}


void TGo4CommandLine::FileSearchDialog()
{
   QFileDialog fd( this, "Select ROOT macro to execute in GUI", QString(),
                  "ROOT macro  (*.C *.c);;Go4 hotstart script (*.hotstart)");
   fd.setFileMode( QFileDialog::ExistingFile);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString cmd;
   if(fd.selectedNameFilter().contains(".hotstart"))
      cmd = flst[0];
   else
      cmd = QString(".x ") + flst[0];
   if (InputLine->findText(cmd)<0) InputLine->addItem(cmd);
   InputLine->setCurrentIndex(InputLine->findText(cmd));
}

void TGo4CommandLine::ExecuteSlot()
{
   enterPressedSlot();
}


void TGo4CommandLine::enterPressedSlot()
{
   QString str = InputLine->currentText();
   if (str.length()==0) return;

   if(str.contains("help") || str.contains(".go4h")) {
      PrintHelp();
   } else
   if(str.contains(".hotstart") && !str.contains(".x")) {
      StatusMessage(QString("Executing hotstart script: ") + str);
      StartHotstart(str.toAscii().constData());
   } else {
      StatusMessage(QString("Executing command: ") + str);
      gROOT->ProcessLineSync(str.toAscii().constData());
   }

   go4sett->setCommandsHistoryGUI(InputLine->getHistory(50));
}

void TGo4CommandLine::LoadHistory()
{
// read command history from settings:
    QStringList histlist = go4sett->getCommandsHistoryGUI();
    InputLine->addItems(histlist);
// prepared pre-loading of system macros:
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/corrhistos.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/hishisto.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/addhistos.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/divhistos.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/profileX.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/profileY.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/projectionX.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/projectionY.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/rebin.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/scalex.C").Data()));
    gROOT->ProcessLineSync(Form(".L %s", TGo4Log::subGO4SYS("macros/fft.C").Data()));

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

   std::cout <<"\t- TObject* go4->GetObject(const char* itemname);"<< std::endl;
   std::cout <<"\t\t: get browser object by full pathname. " << std::endl;
   std::cout <<"\t- TString go4->FindItem(const char* name); " << std::endl;
   std::cout <<"\t\t: get full pathname of browser object by name " << std::endl;
   std::cout <<"\t- TString go4->SaveToMemory(const char* path, TObject* obj, Bool_t ownership = kFALSE);"<< std::endl;
   std::cout <<"\t\t: Put object obj into workspace memory under subfolder path. Returns full item pathname. " << std::endl;
   std::cout <<"\t- ViewPanelHandle go4->StartViewPanel()"<< std::endl;
   std::cout <<"\t\t: Open new Go4 viewpanel with default size. Returns handle of new panel." << std::endl;
   std::cout <<"\t- Bool_t go4->DrawItem(const char* itemname, ViewPanelHandle panel = 0, const char* drawopt = 0);"<< std::endl;
   std::cout <<"\t\t: Draw object of full name itemname onto viewpanel panel. Will open new viewpanel if panel not specified. \n\t  ROOT drawoption may be set. " << std::endl;
   std::cout <<"\t- go4->OpenFile(const char* fname)" << std::endl;
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
