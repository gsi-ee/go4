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

#include "TGo4AnalysisWindow.h"

#include <iostream>

#include "TSystem.h"
#include "TList.h"

#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtCore/QProcess>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QFileDialog>
#include <QToolButton>
#include <QDateTime>
#include <QLabel>
#include <QCloseEvent>

#include "TGo4QSettings.h"
#include "TGo4AnalysisProxy.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisObjectResult.h"
#include "QGo4CommandsHistory.h"

TGo4AnalysisWindow::TGo4AnalysisWindow(QWidget *parent, const char *name, bool needoutput, bool needkillbtn) :
    QGo4Widget( parent, name)
{
   setCanDestroyWidget(false);
   setAcceptDrops(false);
   fAnalysisProcess = nullptr;
   fxOutput = nullptr;
   outputBuffer = "";
   fiMaxOuputSize = 0;
   fbShowTimestamps = kFALSE;
   fxTimeFormat = "yyyy-MM-dd hh:mm:ss";
   fxCmdHist = nullptr;
   fHasLink = false;
   fTerminateOnClose = false;

   fNewObjectForEditor = true;

   setWindowTitle("Analysis Terminal");

   setFont(go4sett->getTermFont());

   if (needoutput) {

      resize(700, 400);
      setWindowIcon(QIcon(":/icons/analysiswin.png"));
      QGridLayout *layout = new QGridLayout(this);
      layout->setContentsMargins(11, 11, 11, 11);
      layout->setSpacing(6);

      fxOutput = new QTextEdit();
      fxOutput->setUndoRedoEnabled(false);
      fxOutput->setAutoFormatting(QTextEdit::AutoNone);
      fxOutput->setWordWrapMode(QTextOption::NoWrap);
      fxOutput->setReadOnly(true);
      layout->addWidget(fxOutput, 0, 0);

      fiMaxOuputSize = go4sett->getTermHistorySize();
      UpdateTimeStampFormat();

      QHBoxLayout *box1 = new QHBoxLayout();
      box1->addWidget(new QLabel("Press enter to execute.", this), 1);
      CreateCmdLine(box1);
      layout->addLayout(box1, 1, 0);

      QHBoxLayout *box2 = new QHBoxLayout();
      CreateButtons(box2, needkillbtn);
      layout->addLayout(box2, 2, 0);

      updateTerminalOutput();
   } else {

      QHBoxLayout *box = new QHBoxLayout(this);

      CreateButtons(box, needkillbtn);

      CreateCmdLine(box);

      adjustSize();
   }
}

void TGo4AnalysisWindow::UpdateTimeStampFormat()
{
   fbShowTimestamps = go4sett->getTermShowTimestamp();
   fxTimeFormat= go4sett->getTermTimeFormat();
}


void TGo4AnalysisWindow::CreateCmdLine(QHBoxLayout* box)
{
   fxCmdHist = new QGo4CommandsHistory(this, "commandslist");
   fxCmdHist->setToolTip("CINT command for analysis process. Note: '@' means 'TGo4Analysis::Instance()->' . A leading '$' will invoke Python script.");
   QObject::connect(fxCmdHist, &QGo4CommandsHistory::enterPressedSingal, this, &TGo4AnalysisWindow::CommandSlot);
   fxCmdHist->setMinimumSize( QSize( 220, 25 ) );

   QStringList histlist = go4sett->getCommandsHistoryAnalysis();
   fxCmdHist->addItems(histlist);
   fxCmdHist->setEditText(QString());

   box->addWidget(fxCmdHist, HasOutput() ? 3 : 1);

   QToolButton* MacroSearch = new QToolButton( this );
   MacroSearch->setMinimumSize( QSize( 30, 25 ) );
   MacroSearch->setMaximumSize( QSize( 30, 25 ) );
   MacroSearch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   MacroSearch->setIcon( QIcon(":/icons/findfile.png" ) );
   MacroSearch->setToolTip("Search root or python macro on disk.");
   QObject::connect(MacroSearch, &QToolButton::clicked, this, &TGo4AnalysisWindow::FileDialog_Macro);
   box->addWidget(MacroSearch,1);
}

void TGo4AnalysisWindow::CreateButtons(QHBoxLayout* box, bool needkillbtn)
{
   if (needkillbtn) {
      QToolButton* KillProcess = new QToolButton( this );
      KillProcess->setMinimumSize( QSize( 30, 25 ) );
      KillProcess->setMaximumSize( QSize( 30, 25 ) );
      KillProcess->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      KillProcess->setIcon( QIcon( ":/icons/killanal.png" ) );
      KillProcess->setToolTip("Apply Ctrl+C in the analysis terminal.");
      QObject::connect(KillProcess, &QToolButton::clicked, this, &TGo4AnalysisWindow::RequestTerminate);
      box->addWidget(KillProcess);
   }

   if (HasOutput()) {
      QToolButton* ClearButton = new QToolButton( this );
      ClearButton->setMinimumSize( QSize( 30, 25 ) );
      ClearButton->setMaximumSize( QSize( 30, 25 ) );
      ClearButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      ClearButton->setIcon( QIcon( ":/icons/clear.png" ) );
      ClearButton->setToolTip("Clear Terminal Window.");
      QObject::connect(ClearButton, &QToolButton::clicked, this, &TGo4AnalysisWindow::ClearAnalysisOutput);
      box->addItem(new QSpacerItem(1,1));
      box->addWidget(ClearButton,1);

      QToolButton* ScrollEndButton = new QToolButton( this );
      ScrollEndButton->setMinimumSize( QSize( 30, 25 ) );
      ScrollEndButton->setMaximumSize( QSize( 30, 25 ) );
      ScrollEndButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      ScrollEndButton->setIcon( QIcon( ":/icons/shiftdown.png" ) );
      ScrollEndButton->setToolTip("Scroll to end of Terminal Window. (keyboard: end)");
      QObject::connect(ScrollEndButton, &QToolButton::clicked, this, &TGo4AnalysisWindow::ScrollEndAnalysisOutput);
      box->addWidget(ScrollEndButton,1);
   }

   QToolButton* PrintHistoButton = new QToolButton( this );
   PrintHistoButton->setMinimumSize( QSize( 30, 25 ) );
   PrintHistoButton->setMaximumSize( QSize( 30, 25 ) );
   PrintHistoButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   PrintHistoButton->setIcon( QIcon( ":/icons/hislist.png" ) );
   PrintHistoButton->setToolTip("Print list of all histograms.");
   QObject::connect(PrintHistoButton, &QToolButton::clicked, this, &TGo4AnalysisWindow::PrintHistograms);
   box->addWidget(PrintHistoButton,1);

   QToolButton* PrintConnyButton = new QToolButton( this );
   PrintConnyButton->setMinimumSize( QSize( 30, 25 ) );
   PrintConnyButton->setMaximumSize( QSize( 30, 25 ) );
   PrintConnyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   PrintConnyButton->setIcon( QIcon( ":/icons/condlist.png" ) );
   PrintConnyButton->setToolTip("Print list of all conditions.");
   QObject::connect(PrintConnyButton, &QToolButton::clicked, this, &TGo4AnalysisWindow::PrintConditions);
   box->addWidget(PrintConnyButton,1);

   QToolButton*  PrintEventButton = new QToolButton( this );
   PrintEventButton->setMinimumSize( QSize( 30, 25 ) );
   PrintEventButton->setMaximumSize( QSize( 30, 25 ) );
   PrintEventButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   PrintEventButton->setIcon( QIcon( ":/icons/zoom.png" ) );
   PrintEventButton->setToolTip("Start Event Inspection panel");
   QObject::connect(PrintEventButton, &QToolButton::clicked, this, &TGo4AnalysisWindow::PrintEvent);
   box->addWidget(PrintEventButton,1);
}

TGo4AnalysisWindow::~TGo4AnalysisWindow()
{
   //if (Browser())
   //   Browser()->ToggleMonitoring(0);
   CloseMDIParentSlot(); // JAM remove top level window when changing connection from client to server
}


bool TGo4AnalysisWindow::HasOutput()
{
   return fxOutput != nullptr;
}

void TGo4AnalysisWindow::SetHistorySize(int sz)
{
   fiMaxOuputSize = sz;
}

void TGo4AnalysisWindow::updateTerminalOutput()
{
   if (!fxOutput)
      return;

   unsigned int buflen = outputBuffer.length();

   if (fiMaxOuputSize > 0) {

      // size remaining after cut of text
      unsigned int cutlength = fiMaxOuputSize / 2;

      if (buflen > 0) {
         unsigned int outlen = fxOutput->toPlainText().length();
         if (buflen + outlen < fiMaxOuputSize) {
            fxOutput->append(outputBuffer);
            // fxOutput->moveCursor(QTextCursor::End);
            //  ^JAM just for test, we dont keep this since one may want inspect history during new printouts
         } else if (buflen >= cutlength) {
            outputBuffer.remove(0, buflen - cutlength);
            fxOutput->setText(outputBuffer);
            fxOutput->moveCursor(QTextCursor::End);
         } else {
            QString curr = fxOutput->toPlainText();
            curr.remove(0, cutlength - buflen);
            curr += outputBuffer;
            fxOutput->setText(curr);
            fxOutput->moveCursor(QTextCursor::End);
         }
      }
   } else {
      if (buflen > 0) {
         fxOutput->append(outputBuffer);
         // fxOutput->moveCursor(QTextCursor::End);
         //  ^JAM just for test, we dont keep this since one may want inspect history during new printouts
      }
   }
   outputBuffer = "";
   QTimer::singleShot(100, this, &TGo4AnalysisWindow::updateTerminalOutput);
}

void TGo4AnalysisWindow::readFromStdout()
{
   if (fAnalysisProcess) {
      QByteArray ba = fAnalysisProcess->readAllStandardOutput();
      QString buf(ba);
      AppendOutputBuffer(buf,0);
   }
}


void TGo4AnalysisWindow::readFromStderr()
{
   if (fAnalysisProcess) {
      QByteArray ba = fAnalysisProcess->readAllStandardError();
      QString buf(ba);
      AppendOutputBuffer(buf, 1);
   }
}

void TGo4AnalysisWindow::AddTimeStamp(QString& buf, int prio)
{
   QString pre = prio > 0 ? "!" : "*";
   QString format = QString("GO4-%1> [%2]\n").arg(pre).arg(fxTimeFormat);
   buf = QDateTime::currentDateTime().toString(format) + buf;
}

void TGo4AnalysisWindow::AppendOutputBuffer(const QString& value, int prio)
{
   if (fbShowTimestamps || prio > 1) {
      QString buf = value;
      AddTimeStamp(buf, prio);
      outputBuffer.append(buf);
   } else {
      outputBuffer.append(value);
   }
}

void TGo4AnalysisWindow::ExtractProgArgs(QString &progname, QStringList &args)
{
   QString quoted;

   int first = progname.indexOf("\"");
   int last = progname.lastIndexOf("\"");
   if ((first > 0) && (last > first)) {
      quoted = progname.mid(first+1, last-first-1);
      progname.resize(first);
   }

#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
   args = progname.split(" ",QString::SkipEmptyParts);
#else
   args = progname.split(" ",Qt::SkipEmptyParts);
#endif
   if (args.size() > 0) {
      progname = args.front();
      args.pop_front();
   }
   if (quoted.length() > 0)
      args.push_back(quoted);
}


void TGo4AnalysisWindow::StartAnalysisShell(const char *text, const char *workdir, bool aschildprocess)
{
   if (fAnalysisProcess)
      delete fAnalysisProcess;

   setWindowTitle("Analysis Terminal");

   // IMPORTANT - process should be child of analysis window
   // to be terminated when analysis window closed or Ctrl-C is pressed

   fAnalysisProcess = new QProcess(aschildprocess ? this : nullptr);
   QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
   fAnalysisProcess->setProcessEnvironment(env);

   QObject::connect(fAnalysisProcess, &QProcess::readyReadStandardOutput, this, &TGo4AnalysisWindow::readFromStdout);
   QObject::connect(fAnalysisProcess, &QProcess::readyReadStandardError, this, &TGo4AnalysisWindow::readFromStderr);
   if (workdir)
      fAnalysisProcess->setWorkingDirectory(workdir);

   QString progname = text;
   QStringList args;

   TGo4AnalysisWindow::ExtractProgArgs(progname, args);

   fAnalysisProcess->start(progname, args);

   if (fAnalysisProcess->state() == QProcess::NotRunning) {
      std::cerr << "Fatal error. Could not start the Analysis" << std::endl;
      TerminateAnalysisProcess();
   } else {
      fTerminateOnClose = aschildprocess;
#ifdef _MSC_VER
      setWindowTitle("Analysis Terminal [with QProcess]");
#else
      setWindowTitle(QString("Analysis Terminal [pid:%1]").arg(fAnalysisProcess->processId()));
#endif
   }
}

void TGo4AnalysisWindow::RequestTerminate()
{
   ServiceCall("TerminateAnalysis");
}

void TGo4AnalysisWindow::TerminateAnalysisProcess()
{
   if (!fAnalysisProcess) return;
   AppendOutputBuffer("\nTerminate process ... \n\n",2);
   fAnalysisProcess->terminate();
   if (fAnalysisProcess->state() == QProcess::Running)
      fAnalysisProcess->kill();
   delete fAnalysisProcess;
   fAnalysisProcess = nullptr;
}

void TGo4AnalysisWindow::ClearAnalysisOutput()
{
   if (fxOutput)
      fxOutput->clear();
}

void TGo4AnalysisWindow::ScrollEndAnalysisOutput()
{
   if (fxOutput)
      fxOutput->moveCursor(QTextCursor::End);
}

void TGo4AnalysisWindow::SaveAnalysisOutput()
{
   if (!fxOutput) return;
   QFileDialog fd(this,
                  "Save analysis terminal output",
                  "", "Plain text (*.txt)");
   fd.setFileMode( QFileDialog::AnyFile );
   fd.setAcceptMode(QFileDialog::AcceptSave);

   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;
   QString fileName = flst[0];

   if(!fileName.endsWith(".txt")) fileName.append(".txt");
   QFile NewFile(fileName);
   NewFile.open( QIODevice::ReadWrite | QIODevice::Append );
   QTextStream t( &NewFile );
   t << fxOutput->toPlainText() << "\n";
   NewFile.close();
}

void TGo4AnalysisWindow::CommandSlot()
{
   QString cmd = fxCmdHist->currentText();
   if (cmd.isEmpty()) return;

   TGo4ServerProxy *anal = GetAnalysis();
   if (anal) {
      anal->ExecuteLine(cmd.toLatin1().constData());
      go4sett->setCommandsHistoryAnalysis(fxCmdHist->getHistory(50));
   }
}


void TGo4AnalysisWindow::FileDialog_Macro()
{
   QFileDialog fd(this, "Select ROOT macro for analysis task", "", "CINT Macro(*.C);;Python script(*.py)");
   fd.setFileMode(QFileDialog::ExistingFile);

   if (fd.exec() != QDialog::Accepted)
      return;
   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty())
      return;
   bool iscint = fd.selectedNameFilter().contains(".C");
   bool ispyth = fd.selectedNameFilter().contains(".py");
   QString cmd;
   if (iscint) {
      cmd = QString(".x ") + flst[0];
   } else if (ispyth) {
      cmd = QString("$") + flst[0];
   } else {
      // never come here, but anyway...
      cmd = QString(".x ") + flst[0];
      if (!cmd.endsWith(".C"))
         cmd.append(".C");
   }
   int index = fxCmdHist->findText(cmd);
   if (index < 0) {
      fxCmdHist->insertItem(-1, cmd);
      index = fxCmdHist->findText(cmd);
   }
   fxCmdHist->setCurrentIndex(index);
}

void TGo4AnalysisWindow::PrintHistograms()
{
   const QString cmd = "@PrintHistograms()";
   int index = fxCmdHist->findText(cmd);
   if (index < 0) {
      fxCmdHist->insertItem(-1, cmd);
      index = fxCmdHist->findText(cmd);
   }
   fxCmdHist->setCurrentIndex(index);
   CommandSlot();
}

void TGo4AnalysisWindow::PrintConditions()
{
   const QString cmd = "@PrintConditions()";
   int index = fxCmdHist->findText(cmd);
   if (index < 0) {
      fxCmdHist->insertItem(-1, cmd);
      index = fxCmdHist->findText(cmd);
   }
   fxCmdHist->setCurrentIndex(index);
   CommandSlot();
}

void TGo4AnalysisWindow::PrintEvent()
{
   ServiceCall("StartEventInfo");
}

void TGo4AnalysisWindow::WorkWithDebugOutput(TGo4Slot *slot)
{
   // can be called only once when window is created
   AddLink(slot, "DebugOutput");
   fHasLink = true;
}

void TGo4AnalysisWindow::WorkWithUpdateObjectCmd(TGo4Slot *slot)
{
   // can be called only once when window is created
   AddLink(slot, "ObjectUpdateCmd");
   fHasLink = true;
}

void TGo4AnalysisWindow::WaitForNewObject(bool isobjectforeditor)
{
   fNewObjectForEditor = isobjectforeditor;
}

void TGo4AnalysisWindow::linkedObjectUpdated(const char *linkname, TObject *obj)
{
   if (strcmp(linkname, "ObjectUpdateCmd") == 0) {
      TGo4AnalysisObjectResult* res = dynamic_cast<TGo4AnalysisObjectResult*>(obj);
      if (!res) return;
      Browser()->SyncBrowserSlots();
      const char *itemname = res->GetObjectFullName();
      TClass *cl = Browser()->ItemClass(itemname);
      if (cl) InformThatObjectCreated(itemname, cl);
      if (!fNewObjectForEditor) EditItem(itemname);
      fNewObjectForEditor = true;
   }

   if (strcmp(linkname, "DebugOutput") == 0) {

      TList *lst = dynamic_cast<TList *> (obj);

      TListIter iter(lst, kFALSE);

      while (auto obj = iter()) {
         if (obj == lst->First()) continue;
         AppendOutputBuffer(obj->GetName());
         AppendOutputBuffer("\n");
      }
   }
}

void TGo4AnalysisWindow::linkedObjectRemoved(const char *linkname)
{
   fHasLink = false;

   if (!HasOutput() || (strcmp(linkname, "DebugOutput") == 0))
      ServiceCall("CloseAnalysisWindow");
}

void TGo4AnalysisWindow::resizeEvent(QResizeEvent *e)
{
   // store size of top widget - JAM only if not within dock window (analysis server)
   // size of top widget will be restored when new panel is created
   if (HasOutput())
      go4sett->storePanelSize(parentWidget(), "AnalysisWindow");
}

void TGo4AnalysisWindow::closeEvent(QCloseEvent *e)
{
   e->ignore(); // destroying this would mix up the upper level management
   QWidget *mdi = parentWidget();
   if (mdi)
      mdi->hide(); // instead of destroying, we just hide it when X is clicked. JAM
}
