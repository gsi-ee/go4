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
       void StartAnalysisShell(const char* cmd, const char* workdir = 0);
       void TerminateAnalysisProcess();
       void AppendOutputBuffer(const QString& value);
       void WorkWithUpdateObjectCmd(TGo4Slot* slot);
       void WaitForNewObject(bool isobjectforeditor);

       bool HasOutput();

   public slots:
      void readFromStdout();
      void readFromStderr();
      void RequestTerminate();
      void ClearAnalysisOutput();
      void SaveAnalysisOutput();
      void HistActivated(const QString& str);
      void FileDialog_Macro();
      void PrintHistograms();
      void PrintConditions();
      void PrintEvent();

      void updateTerminalOutput();

   protected:

      virtual void linkedObjectUpdated(const char* linkname, TObject* obj);
      virtual void linkedObjectRemoved(const char* linkname);

      void CreateButtons(QHBoxLayout*, bool);
      void CreateCmdLine(QHBoxLayout*);

      QProcess *fAnalysisProcess;
      QGo4CommandsHistory *fxCmdHist;
      QString outputBuffer;
      QTextEdit* fxOutput;
      unsigned int fiMaxOuputSize;
      bool fNewObjectForEditor;
};

#endif // TGO4ANALYSISWINDOW_H
