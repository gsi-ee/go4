// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4ANALYSISWINDOW_H
#define TGO4ANALYSISWINDOW_H

/** @author Mohammad Al-Turany
  * @author Sergey Linev
  * @since 06.09.2002 */

#include "QGo4Widget.h"

class QProcess;
class QGo4CommandsHistory;
class QTextEdit;
class QHBoxLayout;

class TGo4AnalysisWindow : public QGo4Widget {
   Q_OBJECT

   public:
       TGo4AnalysisWindow( QWidget* parent = 0, const char* name = 0, bool needoutput = false, bool needkillbtn = true);
       virtual ~TGo4AnalysisWindow();

       void SetHistorySize(int sz);
       void StartAnalysisShell(const char* cmd, const char* workdir = 0, bool aschildprocess = false);
       void TerminateAnalysisProcess();
       void AppendOutputBuffer(const QString& value, int prio=0);
       void UpdateTimeStampFormat();
       void WorkWithUpdateObjectCmd(TGo4Slot* slot);
       void WorkWithDebugOutput(TGo4Slot* slot);
       void WaitForNewObject(bool isobjectforeditor);

       bool HasOutput();
       bool HasLink() const { return fHasLink; }
       bool TerminateOnClose() const { return fTerminateOnClose; }

   public slots:
      void readFromStdout();
      void readFromStderr();
      void RequestTerminate();
      void ClearAnalysisOutput();
      void ScrollEndAnalysisOutput();
      void SaveAnalysisOutput();
      void FileDialog_Macro();
      void PrintHistograms();
      void PrintConditions();
      void PrintEvent();
      void CommandSlot();

      void updateTerminalOutput();

   protected:

      virtual void linkedObjectUpdated(const char* linkname, TObject* obj);
      virtual void linkedObjectRemoved(const char* linkname);

      virtual void resizeEvent(QResizeEvent *);
      virtual void closeEvent(QCloseEvent* e);
      void CreateButtons(QHBoxLayout*, bool);
      void CreateCmdLine(QHBoxLayout*);



      /** prepend timestamp in front of next terminal buffer.
       * Depending on prio, stderr output can be marked as warning*/
      void AddTimeStamp(QString& buf, int prio=0);

      QProcess *fAnalysisProcess;
      QGo4CommandsHistory *fxCmdHist;
      QString outputBuffer;
      QTextEdit* fxOutput;
      unsigned int fiMaxOuputSize;
      bool fbShowTimestamps;
      QString fxTimeFormat;
      bool fNewObjectForEditor;
      bool fHasLink;
      bool fTerminateOnClose;
};

#endif // TGO4ANALYSISWINDOW_H
