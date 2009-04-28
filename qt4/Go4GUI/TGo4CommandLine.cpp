#include "TGo4CommandLine.h"

#include "Riostream.h"
#include "TROOT.h"

#include <QFileDialog>
#include <QTextStream>

#include "TGo4Script.h"
#include "TGo4MainWindow.h"
#include "TGo4QSettings.h"
#include "TGo4MacroDialog.h"

TGo4CommandLine::TGo4CommandLine(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections
			// from ui file to the setupUI
    fiHistoryDepth=50;
    LoadHistory();
}


void TGo4CommandLine::FileSearchDialog()
{
   QFileDialog fd( this, "Select ROOT macro to execute in GUI", QString(),
                  "ROOT macro  (*.C *.c);;Go4 hotstart script (*.hotstart)");
   fd.setMode( QFileDialog::ExistingFile);

   if (fd.exec() != QDialog::Accepted) return;
   QString cmd;
   if(fd.selectedFilter().contains(".hotstart"))
      cmd = fd.selectedFile();
   else
      cmd = QString(".x ") + fd.selectedFile();
   InputLine->addItem(cmd);
}


void TGo4CommandLine::SelectCommand( const QString & str )
{
if (InputLine->EnterPressed() && (str!="")) {
      InputLine->ResetEnterPressed();
      InputLine->removeItem(InputLine->currentIndex()); // remove possible duplicate
      int pos = -1;
      for (int i=0;i<InputLine->count();i++){
            if (InputLine->itemText(i)=="") pos = i;
        }
        if (pos>=0) InputLine->removeItem(pos); // clear previous blank line
      InputLine->addItem(str); // put last command on top of history
      InputLine->addItem(""); // insert blank line on top
      if (InputLine->currentIndex()!=0)
        InputLine->setCurrentIndex(0); // provide free line
      QString message;
       if(str.contains("help") || str.contains(".go4h"))
        {
            PrintHelp();
        }
      else if(str.contains(".hotstart") && !str.contains(".x"))
        {
            QTextOStream( &message ) <<"Executing hotstart script: "<<str.latin1() << endl;
            StatusMessage(message);
            fxMainWindow->HotStart(str.latin1());
        }
      else
        {
            QTextOStream( &message ) <<"Executing command: "<<str.latin1() << endl;
            StatusMessage(message);
            gROOT->ProcessLine(str.latin1());
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
            //cout <<"\n Save history saved "<<InputLine->text(i) << endl;
            histlist.append(InputLine->itemText(i));
        }
    go4sett->setCommandsHistoryGUI(histlist);
}


void TGo4CommandLine::LoadHistory()
{
// read command history from settings:
    QStringList histlist=go4sett->getCommandsHistoryGUI();
    InputLine->addItems(histlist);
// prepared pre-loading of system macros:
    gROOT->ProcessLine(".L $GO4SYS/macros/corrhistos.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/hishisto.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/addhistos.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/divhistos.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/profileX.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/profileY.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/projectionX.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/projectionY.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/rebin.C");
    gROOT->ProcessLine(".L $GO4SYS/macros/scalex.C");
}


void TGo4CommandLine::PredefinedDialog()
{
   TGo4MacroDialog md( this, "select", true);
   if (md.exec() != QDialog::Accepted) return;
   InputLine->addItem(md.getCommand());
   InputLine->setCurrentIndex(0);
}


void TGo4CommandLine::PrintHelp()
{
cout <<"\n--- Go4 GUI command line short help --- " << endl;
cout <<"- execute any command by pressing RETURN (Enter key)" << endl;
cout <<"--" << endl;
cout <<"- use '.h' for help concerning ROOT commands" << endl;
cout <<"--" << endl;
cout <<"- use 'go4->...' to access TGo4AbstractInterface for gui commands" << endl;
cout <<"--" << endl;
cout <<"- Some useful Go4 GUI command functions:" << endl;

cout <<"\t- TObject* go4->GetObject(const char* itemname);"<< endl;
cout <<"\t\t: get browser object by full pathname. " << endl;
cout <<"\t- TString go4->FindItem(const char* name); " << endl;
cout <<"\t\t: get full pathname of browser object by name " << endl;
cout <<"\t- TString go4->SaveToMemory(const char* path, TObject* obj, Bool_t ownership = kFALSE);"<< endl;
cout <<"\t\t: Put object obj into workspace memory under subfolder path. Returns full item pathname. " << endl;
cout <<"\t- ViewPanelHandle go4->StartViewPanel()"<< endl;
cout <<"\t\t: Open new Go4 viewpanel with default size. Returns handle of new panel." << endl;
cout <<"\t- Bool_t go4->DrawItem(const char* itemname, ViewPanelHandle panel = 0, const char* drawopt = 0);"<< endl;
cout <<"\t\t: Draw object of full name itemname onto viewpanel panel. Will open new viewpanel if panel not specified. \n\t  ROOT drawoption may be set. " << endl;
cout <<"\t- go4->OpenFile(const char* fname)" << endl;
cout <<"\t\t: open ROOT file of fname in Go4 browser " << endl;
cout <<"\t- go4->LaunchAnalysis()" << endl;
cout <<"\t\t: Start Go4 analysis process with previous set up " << endl;
cout <<"--" << endl;
cout <<"---- Please: note the Go4 helpwindow with TGo4AbstractInterface full method documentation!" << endl;
cout <<"----         see example scripts at $GO4SYS/macros !" << endl;
cout <<"----         visit our website at: \t http://go4.gsi.de !" << endl;
cout <<"------ " << endl;
HelpWindow("docs/Go4Reference.pdf", "Show Go4 Reference manual...");
}
