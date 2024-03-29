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

#ifndef TGO4ANALYSISCONFIGURATION_H
#define TGO4ANALYSISCONFIGURATION_H

#include "QGo4Widget.h"
#include "ui_TGo4AnalysisConfiguration.h"

class TGo4ConfigStep;
class TGo4ServerProxy;

class TGo4AnalysisConfiguration : public QGo4Widget, public Ui::TGo4AnalysisConfiguration {
   Q_OBJECT

public:
   TGo4AnalysisConfiguration(QWidget *parent = nullptr, const char *name = nullptr);

   bool IsAcceptDrag(const char *itemname, TClass *cl, int kind) override;

   void DropItem(const char *itemname, TClass *cl, int kind) override;

   void linkedObjectUpdated(const char *linkname, TObject *obj) override;

   void linkedObjectRemoved(const char *linkname) override;

   virtual void WorkWithAnalysis(TGo4ServerProxy *anal);

   void ResetWidget() override;

   virtual void RefreshWidget();

   virtual QString GetSourcePath();

   virtual QString GetStorePath();

   virtual void SetSourcePath(const QString &v);

   virtual void SetStorePath(const QString &v);

   int GetNumSteps();

   void ChangeTabTitle(TGo4ConfigStep *step, int number);

   TGo4ConfigStep *GetStepConfig(int n);

   TGo4ConfigStep *FindStepConfig(QString name);

   virtual void SetAutoSaveConfig(QString filename, int interval, int compression, bool enbaled, bool overwrite);

   virtual void GetAutoSaveConfig(QString &filename, int &interval, int &compression, bool &enbaled, bool &overwrite);

   virtual void SetAnalysisConfigFile(QString filename);

   virtual void GetAnalysisConfigFile(QString &filename);

   virtual void DisplayMbsMonitor(const QString &mbsname);

public slots:

   virtual void RequestAnalysisStatus();

   virtual void FileDialog_ConfFile();

   virtual void FileDialog_AutoSave();

   virtual void LineEdit_AutoSaveFile(const QString &);

   virtual void SetCompressionLevel(int t);

   virtual void LoadConfiguration();

   virtual void SaveConfiguration();

   virtual void SubmitConfiguration();

   virtual void SubmitAndStart();

   virtual void CloseAnalysis();

   virtual void SetAutoSaveInterval(int t);

   virtual void SetAutoSaveOverwrite(bool overwrite);

   virtual void WriteAutoSave();

   virtual void EnableAutoSaveSlot(bool disabled);

protected:
   void closeEvent(QCloseEvent *e) override;

   bool fbTypingMode{false};
   QString fSourcePath;
   QString fStorePath;
   QString fConfigPath;
   QString fAutoSavePath;
};

#endif
