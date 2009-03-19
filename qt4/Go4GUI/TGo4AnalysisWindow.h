#ifndef TGO4ANALYSISWINDOW_H
#define TGO4ANALYSISWINDOW_H

/** @author Mohammad Al-Turany
  * @author Sergey Linev
  * @since 06.09.2002 */

#include "QGo4Widget.h"
#include <Qt3Support/Q3HBoxLayout>

class Q3Process;
class QGo4CommandsHistory;
class Q3TextEdit;
class Q3HBoxLayout;

class TGo4AnalysisWindow : public QGo4Widget {
   Q_OBJECT

   public:
       TGo4AnalysisWindow( QWidget* parent = 0, const char* name = 0, bool needoutput = false, bool needkillbtn = true);
       virtual ~TGo4AnalysisWindow();

       void SetHistorySize(int sz);
       void StartAnalysisShell(const char* text);
       void TerminateAnalysisProcess();
       void AppendOutputBuffer(const QString& value);
       void WorkWithUpdateObjectCmd(TGo4Slot* slot);
       void WaitForNewObject(bool isobjectforeditor);

       bool HasOutput();

   public slots:
      void readFromStdout();
      void readFromStderr();
      void scrollToTop();
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

      void CreateButtons(Q3HBoxLayout*, bool);
      void CreateCmdLine(Q3HBoxLayout*);

      Q3Process *fAnalysisProcess;
      QGo4CommandsHistory *fxCmdHist;
      QString outputBuffer;
      Q3TextEdit* fxOutput;
      unsigned int fiMaxOuputSize;
      bool fNewObjectForEditor;
};

#endif // TGO4ANALYSISWINDOW_H
