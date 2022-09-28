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
       TGo4AnalysisWindow(QWidget* parent = nullptr, const char *name = nullptr, bool needoutput = false, bool needkillbtn = true);
       virtual ~TGo4AnalysisWindow();

       void SetHistorySize(int sz);
       void StartAnalysisShell(const char* cmd, const char* workdir = nullptr, bool aschildprocess = false);
       void TerminateAnalysisProcess();
       void AppendOutputBuffer(const QString& value, int prio = 0);
       void UpdateTimeStampFormat();
       void WorkWithUpdateObjectCmd(TGo4Slot* slot);
       void WorkWithDebugOutput(TGo4Slot* slot);
       void WaitForNewObject(bool isobjectforeditor);

       bool HasOutput();
       bool HasLink() const { return fHasLink; }
       bool TerminateOnClose() const { return fTerminateOnClose; }

       static void ExtractProgArgs(QString &prog, QStringList &args);

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

      void linkedObjectUpdated(const char* linkname, TObject* obj) override;
      void linkedObjectRemoved(const char* linkname) override;

      void resizeEvent(QResizeEvent *) override;
      void closeEvent(QCloseEvent* e) override;
      void CreateButtons(QHBoxLayout*, bool);
      void CreateCmdLine(QHBoxLayout*);

      /** prepend timestamp in front of next terminal buffer.
       * Depending on prio, stderr output can be marked as warning*/
      void AddTimeStamp(QString& buf, int prio = 0);

      QProcess *fAnalysisProcess{nullptr};
      QGo4CommandsHistory *fxCmdHist{nullptr};
      QString outputBuffer;
      QTextEdit* fxOutput{nullptr};
      unsigned int fiMaxOuputSize{0};
      bool fbShowTimestamps{false};
      QString fxTimeFormat;
      bool fNewObjectForEditor{false};
      bool fHasLink{false};
      bool fTerminateOnClose{false};
};

#endif // TGO4ANALYSISWINDOW_H
