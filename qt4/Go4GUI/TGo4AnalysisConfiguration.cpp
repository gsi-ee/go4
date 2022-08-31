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

#include "TGo4AnalysisConfiguration.h"

#include <QFileDialog>
#include <QCloseEvent>

#include "TGo4ConfigStep.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisStepStatus.h"
#include "TGo4ServerProxy.h"


TGo4AnalysisConfiguration::TGo4AnalysisConfiguration(QWidget *parent, const char* name) :
   QGo4Widget(parent, name)
{
   setupUi(this);

   QObject::connect(SubmitPushButton, &QPushButton::pressed, this, &TGo4AnalysisConfiguration::SubmitConfiguration);
   QObject::connect(AnalysisSaveConf, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::SaveConfiguration);
   QObject::connect(LoadConfigFileName, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::FileDialog_ConfFile);
   QObject::connect(LoadAutoSaveFileName, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::FileDialog_AutoSave);
   QObject::connect(AutoSaveFileName, &QLineEdit::textChanged, this, &TGo4AnalysisConfiguration::LineEdit_AutoSaveFile);
   QObject::connect(GetActiveConfigButton, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::RequestAnalysisStatus);
   QObject::connect(CompLevel, QOverload<int>::of(&QSpinBox::valueChanged), this, &TGo4AnalysisConfiguration::SetCompressionLevel);
   QObject::connect(AnalysisLoadConf, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::LoadConfiguration);
   QObject::connect(AutoSaveInterval, QOverload<int>::of(&QSpinBox::valueChanged), this, &TGo4AnalysisConfiguration::SetAutoSaveInterval);
   QObject::connect(AutoSaveOverwrite, &QCheckBox::toggled, this, &TGo4AnalysisConfiguration::SetAutoSaveOverwrite);
   QObject::connect(SaveNowB, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::WriteAutoSave);
   QObject::connect(AutoSaveEnable, &QCheckBox::toggled, this, &TGo4AnalysisConfiguration::EnableAutoSaveSlot);
   QObject::connect(SubmitAndStartButton, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::SubmitAndStart);
   QObject::connect(ClosePushButton, &QPushButton::clicked, this, &TGo4AnalysisConfiguration::CloseAnalysis);

   fSourcePath = ".";
   fStorePath = ".";
   fConfigPath = ".";
   fAutoSavePath = ".";
   fbTypingMode = true;
   setCanDestroyWidget(false);
}


bool TGo4AnalysisConfiguration::IsAcceptDrag(const char *itemname, TClass* cl, int kind)
{
   return false;
//   if (!cl) return false;
//   return cl->InheritsFrom(TGo4AnalysisStatus::Class());
}

void TGo4AnalysisConfiguration::DropItem(const char *itemname, TClass* cl, int kind)
{
//   if (!cl) return;

//   if (cl->InheritsFrom(TGo4AnalysisStatus::Class()))
//      WorkWithStatus(itemname);
}

void TGo4AnalysisConfiguration::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname,"Status") == 0) {
     parentWidget()->raise();
     parentWidget()->show();
     if (parentWidget()->isMinimized()) parentWidget()->showNormal();
     RefreshWidget();

     auto anl = GetLinkedCast<TGo4ServerProxy>("Analysis");
     if (anl)
        anl->RefreshNamesList();
   }
}

void TGo4AnalysisConfiguration::linkedObjectRemoved(const char* linkname)
{
   ShootCloseWidget(true);
}

void TGo4AnalysisConfiguration::WorkWithAnalysis(TGo4ServerProxy* anal)
{
   ResetWidget();

   if (!anal) {
      ShootCloseWidget(true);
      return;
   }

   AddLink(anal->ParentSlot(), "Analysis");
   AddLink(anal->SettingsSlot(), "Status");

   RefreshWidget();
}

void TGo4AnalysisConfiguration::ResetWidget()
{
   QGo4Widget::ResetWidget();

   QWidget* w = nullptr;
   do {
     w = TabSteps->currentWidget();
     if (w) {
        TabSteps->removeTab(TabSteps->indexOf(w));
        delete w;
     }
   } while (w);
}

void TGo4AnalysisConfiguration::RefreshWidget()
{
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   auto anl = GetLinkedCast<TGo4ServerProxy>("Analysis");

   if (!status || !anl) return;

   SubmitPushButton->setEnabled(anl->CanSubmitObjects());
   SubmitAndStartButton->setEnabled(anl->CanSubmitObjects());

   fbTypingMode = false;

   QWidget* w = nullptr;
   do {
     w = TabSteps->currentWidget();
     if (w) {
        TabSteps->removeTab(TabSteps->indexOf(w));
        delete w;
     }
   } while (w);

   for(int i=0; i<status->GetNumberOfSteps(); i++) {
      TGo4AnalysisStepStatus* stepstatus = status->GetStepStatus(i);
      if (!stepstatus) continue;
      QString StepName = stepstatus->GetName();
      TGo4ConfigStep* NewStep = new TGo4ConfigStep(TabSteps, StepName.toLatin1().constData());
      NewStep->SetStepStatus(this, stepstatus, status->GetNumberOfSteps() > 1 ? i : -1);
      TabSteps->insertTab(i, NewStep, NewStep->GetTabTitle());
    }

    TabSteps->setCurrentIndex(0);
    TabSteps->adjustSize();

    SetAutoSaveConfig(status->GetAutoFileName(),
                      status->GetAutoSaveInterval(),
                      status->GetAutoSaveCompression(),
                      status->IsAutoSaveOn(),
                      status->IsAutoSaveOverwrite());

    SetAnalysisConfigFile(status->GetConfigFileName());

    parentWidget()->showNormal();
    parentWidget()->adjustSize();

    fbTypingMode = true;
}

void TGo4AnalysisConfiguration::ChangeTabTitle(TGo4ConfigStep* step, int number)
{
   if (number >= 0)
      TabSteps->setTabText(number, step->GetTabTitle());
}


QString TGo4AnalysisConfiguration::GetSourcePath()
{
   return fSourcePath;
}

QString TGo4AnalysisConfiguration::GetStorePath()
{
   return fStorePath;
}

void TGo4AnalysisConfiguration::SetSourcePath(const QString & v)
{
   fSourcePath = v;
}

void TGo4AnalysisConfiguration::SetStorePath(const QString & v)
{
   fStorePath = v;
}

void TGo4AnalysisConfiguration::RequestAnalysisStatus()
{
   auto anl = GetLinkedCast<TGo4ServerProxy>("Analysis");
   if (anl)
      anl->RequestAnalysisSettings();
}

void TGo4AnalysisConfiguration::FileDialog_ConfFile()
{
    QFileDialog fd(this,
                   "Select a configuration file",
                   fConfigPath,
                   " Configuration file (*.root)");
    fd.setFileMode( QFileDialog::AnyFile);
    if (fd.exec() != QDialog::Accepted) return;
    QStringList flst = fd.selectedFiles();
    QString fileName = flst.isEmpty() ? QString() : flst[0];
    fConfigPath = fd.directory().path();
    if(!fileName.endsWith(".root")) fileName.append(".root");
    ConfigFileName->setText(fileName);
}

void TGo4AnalysisConfiguration::FileDialog_AutoSave()
{
    QFileDialog fd( this,
          "Select file name for autosaving",
          fAutoSavePath,
          "Auto Save File (*.root)");
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if (fd.exec() != QDialog::Accepted)  return;

    QStringList flst = fd.selectedFiles();
    if (flst.isEmpty()) return;

    QString fileName = flst[0];
    fAutoSavePath = fd.directory().path();
    if(!fileName.endsWith(".root")) fileName.append(".root");
    AutoSaveFileName->setText(fileName);
    LineEdit_AutoSaveFile("");
}

void TGo4AnalysisConfiguration::LineEdit_AutoSaveFile(const QString &)
{
   QString fname = AutoSaveFileName->text().trimmed();
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   if (status && fbTypingMode)
      status->SetAutoFileName(fname.toLatin1().constData());
}

void TGo4AnalysisConfiguration::SetCompressionLevel( int t)
{
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   if (status && fbTypingMode)
      status->SetAutoSaveCompression(t);
}

void TGo4AnalysisConfiguration::LoadConfiguration()
{
   QString fname = ConfigFileName->text().trimmed();
   auto anl = GetLinkedCast<TGo4ServerProxy>("Analysis");
   if (anl)
      anl->LoadConfigFile(fname.toLatin1().constData());
   RequestAnalysisStatus();
}

void TGo4AnalysisConfiguration::SaveConfiguration()
{
   QString fname = ConfigFileName->text().trimmed();
   auto anl = GetLinkedCast<TGo4ServerProxy>("Analysis");
   if (anl)
      anl->SaveConfigFile(fname.toLatin1().constData());
}

void TGo4AnalysisConfiguration::SubmitConfiguration()
{
   ServiceCall("SubmitAnalysisSettings");
}

void TGo4AnalysisConfiguration::SubmitAndStart()
{
   ServiceCall("SubmitStartAnalysis");
   parentWidget()->hide();
}

void TGo4AnalysisConfiguration::CloseAnalysis()
{
   ServiceCall("CloseAnalysisSettings");
}

void TGo4AnalysisConfiguration::SetAutoSaveInterval(int t)
{
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   if (status && fbTypingMode)
     status->SetAutoSaveInterval(t);
}

void TGo4AnalysisConfiguration::SetAutoSaveOverwrite(bool overwrite)
{
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   if (status && fbTypingMode)
      status->SetAutoSaveOverwrite(overwrite);
}

void TGo4AnalysisConfiguration::WriteAutoSave()
{
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   auto anl = GetLinkedCast<TGo4ServerProxy>("Analysis");

   if (anl && status)
     anl->WriteAutoSave(status->GetAutoFileName(),
                        status->GetAutoSaveCompression(),
                        status->IsAutoSaveOverwrite());
}

void TGo4AnalysisConfiguration::EnableAutoSaveSlot(bool enabled)
{
   auto status = GetLinkedCast<TGo4AnalysisStatus>("Status");
   if (status && fbTypingMode)
       status->SetAutoSaveOn(enabled);
    AutoSaveInterval->setEnabled(enabled);
}

int TGo4AnalysisConfiguration::GetNumSteps()
{
   return TabSteps->count();
}

TGo4ConfigStep* TGo4AnalysisConfiguration::GetStepConfig(int n)
{
   if ((n<0) || (n>=TabSteps->count())) return nullptr;
   return dynamic_cast<TGo4ConfigStep*> (TabSteps->widget(n));
}

TGo4ConfigStep* TGo4AnalysisConfiguration::FindStepConfig(QString name)
{
   for (int n=0;n<GetNumSteps();n++) {
      TGo4ConfigStep* conf = GetStepConfig(n);
      if (conf)
         if (conf->GetStepName()==name) return conf;
   }
   return nullptr;
}

void TGo4AnalysisConfiguration::SetAutoSaveConfig(QString filename,
                                                  int interval,
                                                  int compression,
                                                  bool enabled,
                                                  bool overwrite)
{
  AutoSaveFileName->setText(filename);
  AutoSaveInterval->setValue(interval);
  CompLevel->setValue(compression);

  AutoSaveOverwrite->setChecked(overwrite);
  AutoSaveEnable->setChecked(enabled);

  AutoSaveInterval->setEnabled(enabled);
}

void TGo4AnalysisConfiguration::GetAutoSaveConfig(QString& filename,
                                                  int& interval,
                                                  int& compression,
                                                  bool& enabled,
                                                  bool& overwrite)
{
  filename = AutoSaveFileName->text();
  interval = AutoSaveInterval->value();
  compression = CompLevel->value();

  overwrite = AutoSaveOverwrite->isChecked();
  enabled = AutoSaveEnable->isChecked();
}

void TGo4AnalysisConfiguration::SetAnalysisConfigFile(QString filename)
{
   ConfigFileName->setText(filename);
}

void TGo4AnalysisConfiguration::GetAnalysisConfigFile(QString& filename)
{
   filename = ConfigFileName->text();
}

void TGo4AnalysisConfiguration::DisplayMbsMonitor(const QString& mbsname )
{
   ServiceCall("DisplayMbsMonitor", (void*) mbsname.toLatin1().constData());
}

void TGo4AnalysisConfiguration::closeEvent(QCloseEvent* e)
{
  e->ignore(); // destroying this would mix up the upper level management
  QWidget* mdi = parentWidget();
  if (mdi)
     mdi->hide(); // instead of destroying, we just hide it when X is clicked. JAM
}

