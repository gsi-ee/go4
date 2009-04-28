#include "TGo4AnalysisWindow.h"

#include "TSystem.h"
#include "Riostream.h"

#include "qmime.h"
#include "qlayout.h"
#include "qtooltip.h"
#include "qimage.h"
#include <QtCore/qtimer.h>
#include "qlabel.h"
#include "qmessagebox.h"
#include "qpushbutton.h"
#include "qtoolbutton.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtCore/QTextStream>
#include <QFileDialog>
#include <QtCore/QProcess>
#include <QtGui/QTextEdit>

#include "TGo4QSettings.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisObjectResult.h"
#include "QGo4CommandsHistory.h"



TGo4AnalysisWindow::TGo4AnalysisWindow(QWidget* parent, const char* name, bool needoutput, bool needkillbtn)
    : QGo4Widget( parent, name)
{

    setCanDestroyWidget(false);
    setAcceptDrops(FALSE);

    fAnalysisProcess = 0;
    fxOutput = 0;
    outputBuffer = "";
    fiMaxOuputSize = 0;
    fxCmdHist = 0;

    fNewObjectForEditor = true;

    setCaption("Analysis Terminal");

    if (needoutput) {

       resize(700, 400);
       setWindowIcon(QIcon(":/icons/analysiswin.png"));
       QGridLayout* layout = new QGridLayout( this, 1, 1, 11, 6, "layout");
       fxOutput = new QTextEdit(this, "output");
       fxOutput->setUndoRedoEnabled(FALSE);
       fxOutput->setAutoFormatting(QTextEdit::AutoNone);
       fxOutput->setWordWrapMode(QTextOption::NoWrap);
       fxOutput->setReadOnly(TRUE);
       layout->addWidget( fxOutput, 0, 0 );

       fiMaxOuputSize = go4sett->getTermHistorySize();

       QHBoxLayout *box1 = new QHBoxLayout(layout);
       box1->addWidget(new QLabel("Press enter to execute.", this), 1);
       CreateCmdLine(box1);

       QHBoxLayout *box2 = new QHBoxLayout(layout);
       CreateButtons(box2, needkillbtn);

       updateTerminalOutput();
    } else {

       QHBoxLayout *box = new QHBoxLayout(this, 0, 3);

       CreateButtons(box, needkillbtn);

       CreateCmdLine(box);

       adjustSize();
    }
}

void TGo4AnalysisWindow::CreateCmdLine(QHBoxLayout* box)
{
   fxCmdHist = new QGo4CommandsHistory(this, "commandslist");
   QToolTip::add(fxCmdHist, "CINT command for analysis process. Note: '@' means 'TGo4Analysis::Instance()->' .");
   connect(fxCmdHist, SIGNAL(activated(const QString&)),  this, SLOT(HistActivated(const QString&)));
   fxCmdHist->setMinimumSize( QSize( 220, 25 ) );

   box->addWidget(fxCmdHist, HasOutput() ? 3 : 1);

   QToolButton* MacroSearch = new QToolButton( this, "MacroSearch" );
   MacroSearch->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, MacroSearch->sizePolicy().hasHeightForWidth() ) );
   MacroSearch->setMinimumSize( QSize( 30, 25 ) );
   MacroSearch->setMaximumSize( QSize( 30, 25 ) );
   MacroSearch->setIcon( QIcon(":/icons/findfile.png" ) );
   QToolTip::add(MacroSearch, trUtf8( "Search root macro on disk." ) );
   connect(MacroSearch, SIGNAL(clicked()), this, SLOT(FileDialog_Macro()));
   box->addWidget(MacroSearch,1);
}

void TGo4AnalysisWindow::CreateButtons(QHBoxLayout* box, bool needkillbtn)
{
   if (needkillbtn) {
      QToolButton* KillProcess= new QToolButton(this,"KillProcess");
      KillProcess->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, KillProcess->sizePolicy().hasHeightForWidth() ) );
      KillProcess->setMinimumSize( QSize( 30, 25 ) );
      KillProcess->setMaximumSize( QSize( 30, 25 ) );
      KillProcess->setIcon( QIcon( ":/icons/killanal.png" ) );
      QToolTip::add( KillProcess, trUtf8( "Apply Ctrl+C in the analysis terminal." ) );
      connect(KillProcess, SIGNAL(clicked()), this, SLOT(RequestTerminate()));
      box->addWidget(KillProcess);
   }

   if (HasOutput()) {
      QToolButton* ClearButton = new QToolButton( this, "ClearButton" );
      ClearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, ClearButton->sizePolicy().hasHeightForWidth() ) );
      ClearButton->setMinimumSize( QSize( 30, 25 ) );
      ClearButton->setMaximumSize( QSize( 30, 25 ) );
      ClearButton->setIcon( QIcon( ":/icons/clear.png" ) );
      QToolTip::add(ClearButton, trUtf8( "Clear Terminal Window." ) );
      connect(ClearButton, SIGNAL(clicked()), this, SLOT(ClearAnalysisOutput()));
      box->addItem(new QSpacerItem(1,1));
      box->addWidget(ClearButton,1);
   }

   QToolButton* PrintHistoButton = new QToolButton( this, "PrintHistoButton" );
   PrintHistoButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PrintHistoButton->sizePolicy().hasHeightForWidth() ) );
   PrintHistoButton->setMinimumSize( QSize( 30, 25 ) );
   PrintHistoButton->setMaximumSize( QSize( 30, 25 ) );
   PrintHistoButton->setIcon( QIcon( ":/icons/hislist.png" ) );
   QToolTip::add(PrintHistoButton, trUtf8( "Print list of all histograms." ) );
   connect(PrintHistoButton, SIGNAL(clicked()), this, SLOT(PrintHistograms()));
   box->addWidget(PrintHistoButton,1);

   QToolButton* PrintConnyButton = new QToolButton( this, "PrintConnyButton" );
   PrintConnyButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PrintConnyButton->sizePolicy().hasHeightForWidth() ) );
   PrintConnyButton->setMinimumSize( QSize( 30, 25 ) );
   PrintConnyButton->setMaximumSize( QSize( 30, 25 ) );
   PrintConnyButton->setIcon( QIcon( ":/icons/condlist.png" ) );
   QToolTip::add(PrintConnyButton, trUtf8( "Print list of all conditions." ) );
   connect(PrintConnyButton, SIGNAL(clicked()), this, SLOT(PrintConditions()));
   box->addWidget(PrintConnyButton,1);

   QToolButton*  PrintEventButton = new QToolButton( this, "PrintEventButton" );
   PrintEventButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PrintEventButton->sizePolicy().hasHeightForWidth() ) );
   PrintEventButton->setMinimumSize( QSize( 30, 25 ) );
   PrintEventButton->setMaximumSize( QSize( 30, 25 ) );
   PrintEventButton->setIconSet( QIcon( ":/icons/zoom.png" ) );
   QToolTip::add(PrintEventButton, trUtf8( "Start Event Inspection panel" ) );
   connect(PrintEventButton, SIGNAL(clicked()), this, SLOT(PrintEvent()));
   box->addWidget(PrintEventButton,1);
}

TGo4AnalysisWindow::~TGo4AnalysisWindow()
{
//   TerminateAnalysisProcess();
   if (Browser()!=0)
      Browser()->ToggleMonitoring(0);
}

bool TGo4AnalysisWindow::HasOutput()
{
   return fxOutput!=0;
}


void TGo4AnalysisWindow::SetHistorySize(int sz)
{
   fiMaxOuputSize = sz;
}

void TGo4AnalysisWindow::updateTerminalOutput()
{
    if (fxOutput==0) return;

    unsigned int buflen = outputBuffer.length();

    if (fiMaxOuputSize>0) {

      // size remaining after cut of text
      unsigned int cutlength = fiMaxOuputSize / 2;

      if (buflen > 0) {
         unsigned int outlen = fxOutput->toPlainText().length();
         if (buflen + outlen < fiMaxOuputSize)
           fxOutput->append(outputBuffer);
         else
         if (buflen>=cutlength) {
            outputBuffer.remove(0, buflen-cutlength);
            fxOutput->setText(outputBuffer);
            fxOutput->moveCursor(QTextCursor::End);
         } else {
           QString curr = fxOutput->text();
           curr.remove(0, cutlength - buflen);
           curr+=outputBuffer;
           fxOutput->setText(curr);
           fxOutput->moveCursor(QTextCursor::End);
         }
      }
    } else
      if (buflen>0)
         fxOutput->append(outputBuffer);

    outputBuffer = "";
    QTimer::singleShot(100, this, SLOT(updateTerminalOutput()));
}

void TGo4AnalysisWindow::readFromStdout()
{
   if (fAnalysisProcess!=0) {
      QByteArray ba = fAnalysisProcess->readAllStandardOutput();
      QString buf(ba);
      AppendOutputBuffer(buf);
   }
}


void TGo4AnalysisWindow::readFromStderr()
{
    if (fAnalysisProcess!=0) {
       QByteArray ba = fAnalysisProcess->readAllStandardError();
       QString buf(ba);
       AppendOutputBuffer(buf);
    }
}

void TGo4AnalysisWindow::AppendOutputBuffer(const QString& value)
{
    outputBuffer.append(value);
}

void TGo4AnalysisWindow::StartAnalysisShell(const char* text)
{
    if (fAnalysisProcess!=0) delete fAnalysisProcess;

    fAnalysisProcess = new QProcess();
    connect(fAnalysisProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
    connect(fAnalysisProcess, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));

    fAnalysisProcess->start(text);

    if (fAnalysisProcess->state() == QProcess::NotRunning) {
       cerr << "Fatal error. Could not start the Analysis" << endl;
       TerminateAnalysisProcess();
    }
}

void TGo4AnalysisWindow::RequestTerminate()
{
   ServiceCall("TerminateAnalysis");
}

void TGo4AnalysisWindow::TerminateAnalysisProcess()
{
   if (fAnalysisProcess==0) return;
   AppendOutputBuffer("\nTerminate process ... \n\n");
   fAnalysisProcess->terminate();
   if (fAnalysisProcess->state() == QProcess::Running) fAnalysisProcess->kill();
   delete fAnalysisProcess;
   fAnalysisProcess = 0;
}

void TGo4AnalysisWindow::ClearAnalysisOutput()
{
   if (fxOutput!=0)
      fxOutput->clear();
}

void TGo4AnalysisWindow::SaveAnalysisOutput()
{
   if (fxOutput==0) return;
   QFileDialog fd(this,
                  "Save analysis terminal output",
                  "", "Plain text (*.txt)");
   fd.setMode( QFileDialog::AnyFile );

   if (fd.exec() != QDialog::Accepted) return;

   QString fileName = fd.selectedFile();
   if(!fileName.endsWith(".txt")) fileName.append(".txt");
   QFile NewFile(fileName);
   NewFile.open( QIODevice::ReadWrite | QIODevice::Append );
   QTextStream t( &NewFile );
   t << fxOutput->text() << endl;
   NewFile.close();
}

void TGo4AnalysisWindow::HistActivated(const QString& str)
{
   if (fxCmdHist->EnterPressed() && (str!="")) {
      fxCmdHist->ResetEnterPressed();
      int pos = -1;
      for (int i=0;i<fxCmdHist->count();i++)
        if (fxCmdHist->itemText(i)=="") pos = i;

      if (pos>0) fxCmdHist->removeItem(pos);
      if (pos!=0) fxCmdHist->insertItem(0, "");

      if (fxCmdHist->currentIndex()!=0)
        fxCmdHist->setCurrentIndex(0);

      TGo4AnalysisProxy* anal = GetAnalysis();
      if (anal!=0)
        anal->ExecuteLine(str.latin1());
   }
}

void TGo4AnalysisWindow::FileDialog_Macro()
{
   QFileDialog fd( this,
                  "Select ROOT macro for analysis task"
                  "", "CINT Macro  (*.C)");
   fd.setMode( QFileDialog::ExistingFile);

   if (fd.exec() != QDialog::Accepted) return;

   QString cmd = QString(".x ") + fd.selectedFile();
   if(!cmd.endsWith(".C")) cmd.append(".C");
   fxCmdHist->addItem(cmd);
}

void TGo4AnalysisWindow::PrintHistograms()
{
   const QString com="@PrintHistograms()";
   fxCmdHist->addItem(com);
   fxCmdHist->SetEnterPressed(1);
   HistActivated(com);
   fxCmdHist->SetEnterPressed(0);
}

void TGo4AnalysisWindow::PrintConditions()
{
   const QString com="@PrintConditions()";
   fxCmdHist->addItem(com);
   fxCmdHist->SetEnterPressed(1);
   HistActivated(com);
   fxCmdHist->SetEnterPressed(0);
}

void TGo4AnalysisWindow::PrintEvent()
{
   ServiceCall("StartEventInfo");
}

void TGo4AnalysisWindow::WorkWithUpdateObjectCmd(TGo4Slot* slot)
{
   // can be called only once when window is created
   AddLink(slot, "ObjectUpdateCmd");
}

void TGo4AnalysisWindow::WaitForNewObject(bool isobjectforeditor)
{
   fNewObjectForEditor = isobjectforeditor;
}

void TGo4AnalysisWindow::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname, "ObjectUpdateCmd")!=0) return;
   TGo4AnalysisObjectResult* res = dynamic_cast<TGo4AnalysisObjectResult*>(obj);
   if (res==0) return;
   Browser()->SyncBrowserSlots();
   const char* itemname = res->GetObjectFullName();
   TClass* cl = Browser()->ItemClass(itemname);
   if (cl!=0) InformThatObjectCreated(itemname, cl);
   if (!fNewObjectForEditor) EditItem(itemname);
   fNewObjectForEditor = true;
}

void TGo4AnalysisWindow::linkedObjectRemoved(const char* linkname)
{
   if (!HasOutput())
      ServiceCall("CloseAnalysisWindow");
}
