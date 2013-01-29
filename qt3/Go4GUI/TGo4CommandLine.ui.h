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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void TGo4CommandLine::init()
{
    fiHistoryDepth=50;
    LoadHistory();
}


void TGo4CommandLine::FileSearchDialog()
{
QFileDialog fd( this, "search macro", true);
   fd.setCaption("Select ROOT macro to execute in GUI");
   fd.setMode( QFileDialog::ExistingFile);
   fd.setName( "Select macro file");
   QString filters="ROOT macro  (*.C *.c)";
   filters+=";;Go4 hotstart script (*.hotstart)";
   fd.setFilters(filters);
   if (fd.exec() != QDialog::Accepted) return;
   QString cmd;
   if(fd.selectedFilter().contains(".hotstart"))
    {
        cmd=fd.selectedFile();
    }
   else
    {
       cmd = QString(".x ") + fd.selectedFile();
    }
    InputLine->insertItem(cmd, 0);
}


void TGo4CommandLine::SelectCommand( const QString & str )
{
   if (InputLine->EnterPressed() && (str!="")) {
      InputLine->ResetEnterPressed();
      InputLine->removeItem(InputLine->currentItem()); // remove possible duplicate
      int pos = -1;
      for (int i=0;i<InputLine->count();i++){
         if (InputLine->text(i)=="") pos = i;
      }
      if (pos>=0) InputLine->removeItem(pos); // clear previous blank line
      InputLine->insertItem(str,0); // put last command on top of history
      InputLine->insertItem("", 0); // insert blank line on top
      if (InputLine->currentItem()!=0)
         InputLine->setCurrentItem(0); // provide free line
      if(str.contains("help") || str.contains(".go4h"))
      {
         PrintHelp();
      }
      else if(str.contains(".hotstart") && !str.contains(".x"))
      {
         QString message("Executing hotstart script: ");
         message.append(str);
         StatusMessage(message);
         fxMainWindow->HotStart(str.latin1());
      }
      else
      {
         QString message("Executing command: ");
         message.append(str);
         StatusMessage(message);
         gROOT->ProcessLineSync(str.latin1());
      }
      SaveHistory();
   }
}


void TGo4CommandLine::setMainWindow( TGo4MainWindow * win )
{
    fxMainWindow=win;
}


void TGo4CommandLine::SaveHistory()
{
    QStringList histlist;
    for(int i=0;( (i<InputLine->count() ) && (i<fiHistoryDepth) ); ++i)
    {
       histlist.append(InputLine->text(i));
    }
    go4sett->setCommandsHistoryGUI(histlist);
}


void TGo4CommandLine::LoadHistory()
{
// read command history from settings:
    QStringList histlist=go4sett->getCommandsHistoryGUI();
    InputLine->insertStringList(histlist);
// prepared pre-loading of system macros:
    TString dir = TGo4Log::subGO4SYS("macros/");

    gROOT->ProcessLineSync(Form(".L %scorrhistos.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %shishisto.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %saddhistos.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sdivhistos.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sprofileX.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sprofileY.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sprojectionX.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sprojectionY.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %srebin.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sscalex.C", dir.Data()));
    gROOT->ProcessLineSync(Form(".L %sfft.C", dir.Data()));
}

void TGo4CommandLine::PredefinedDialog()
{
   TGo4MacroDialog md( this, "select", true);
   if (md.exec() != QDialog::Accepted) return;
   QString cmd=md.getCommand();
   InputLine->insertItem(cmd, 0);
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
   HelpWindow("docs/Go4Reference.pdf");
}
