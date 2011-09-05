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

#include "TGo4ConfigStep.h"

#include <QFileDialog>

#include "TClass.h"
#include "Riostream.h"

#include "TGo4AnalysisStepStatus.h"
#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"
#include "TGo4UserSourceParameter.h"
#include "TGo4RevServParameter.h"
#include "TGo4FileStoreParameter.h"
#include "TGo4BackStoreParameter.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4MbsFileParameter.h"
#include "TGo4MbsStreamParameter.h"
#include "TGo4MbsTransportParameter.h"
#include "TGo4MbsEventServerParameter.h"
#include "TGo4MbsRandomParameter.h"
#include "Go4EventServerTypes.h"
#include "TGo4AnalysisConfiguration.h"


const char* TGo4MbsFile__fgcNOTAGFILE = "GO4-NOLMDTAG";
const char* TGo4MbsFile__fgcLMDSUF = ".lmd";
const char* TGo4MbsFile__fgcFILELISTSUF = ".lml";

TGo4ConfigStep::TGo4ConfigStep( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, fl )
{
   setObjectName( name ? name : "Go4ConfigStep");

   setupUi(this);

   fxPanel = 0;
   fStepStatus = 0;

   SpinBoxPortNumber->setShown(false);
   TextLabelPortNumber->setShown(false);
   LineEditArgs->setShown(false);
   TextLabelArgs->setShown(false);
   LineEditTagfile->setShown(false);
   TextLabelTagfile->setShown(false);
}


void TGo4ConfigStep::InputArguments(const QString& Arg)
{
    TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
    int ParId=SourcePar->GetID();
    if (ParId==GO4EV_USER){                    // user defined source class
        TGo4UserSourceParameter* usrpar=dynamic_cast<TGo4UserSourceParameter*>(SourcePar);
        if (usrpar!=0)
           usrpar->SetExpression(Arg.trimmed().toAscii().constData());
    }
}

void TGo4ConfigStep::InputStateChanged(int )
{
   bool stepon = EnableStepBox->isChecked();
   bool on = EnableSourceBox->isChecked();

   fStepStatus->SetSourceEnabled(on);
   SourceBox->setEnabled(stepon && on);

   fxPanel->ChangeTabTitle(this, fStepNumber);
}

void TGo4ConfigStep::InputPortChanged(int port)
{
   TGo4EventSourceParameter* SourcePar = fStepStatus->GetSourcePar();

   switch (SourcePar->GetID()) {

      case GO4EV_MBS_TRANSPORT:
      case GO4EV_MBS_STREAM:
      case GO4EV_MBS_EVENTSERVER:
      case GO4EV_MBS_REVSERV: {
         TGo4MbsSourceParameter* par = dynamic_cast<TGo4MbsSourceParameter*>(SourcePar);
         if (par!=0)
            par->SetPort(port);
         break;
      }

      case GO4EV_USER: { // user defined source class
         TGo4UserSourceParameter* usrpar=dynamic_cast<TGo4UserSourceParameter*>(SourcePar);
         if (usrpar!=0)
            usrpar->SetPort(port);
         break;
      }

      default: {
         // do nothing
         break;
      }
   }
}

void TGo4ConfigStep::InputSourceText(const QString& Name)
{
    fStepStatus->GetSourcePar()->SetName(Name.trimmed().toAscii().constData());
}

void TGo4ConfigStep::OutputStateChanged(int )
{
   bool stepon = EnableStepBox->isChecked();
   bool on = EnableStoreBox->isChecked();

   fStepStatus->SetStoreEnabled(on);
   StoreBox->setEnabled(stepon && on);
   StoreBox->setVisible(on);

   fxPanel->ChangeTabTitle(this, fStepNumber);

   parentWidget()->adjustSize();
   parentWidget()->parentWidget()->adjustSize();
   parentWidget()->parentWidget()->parentWidget()->adjustSize();
}

void TGo4ConfigStep::OutputNameText(const QString& Name)
{
   fStepStatus->GetStorePar()->SetName(Name.trimmed().toAscii().constData());
}

void TGo4ConfigStep::StepStateChanged(int )
{
   bool on = EnableStepBox->isChecked();

   fStepStatus->SetProcessEnabled(on);
   EnableSourceBox->setEnabled(on);
   EnableStoreBox->setEnabled(on);

   fxPanel->ChangeTabTitle(this, fStepNumber);

   InputStateChanged(0);
   OutputStateChanged(0);
}

void TGo4ConfigStep::OutArguments(const QString&)
{
}

void TGo4ConfigStep::SetStepStatus(TGo4AnalysisConfiguration* panel, TGo4AnalysisStepStatus* StepStatus, int number)
{
    if (StepStatus==0) return;
    //cout <<"TGo4ConfigStep::SetStepStatus for "<< StepStatus->GetName()<< endl;

    fxPanel = panel;
    fStepStatus = StepStatus;
    fStepNumber = number;

    //----------------------------------------------------//
    //Source:

    /*
    Qt use following order to display the source parameter:

    0)  root file with one tree                 TGo4FileSourceParameter
    1)  branch of singleton tree                TGo4TreeSourceParameter
    2)  mbs listmode file (input only)          TGo4MbsFileParameter
    3)  mbs stream server (input only)          TGo4MbsStreamParameter
    4)  mbs transport server (input only)       TGo4MbsTransportParameter
    5)  mbs event server  (input only)          TGo4MbsEventServerParameter
    6)  rev serv                                TGo4RevServParameter
    7)  mbs random generator                    TGo4MbsRandomParameter
    8)  user source                             TGo4UserSourceParameter

    */

    TGo4EventSourceParameter* srcpar = StepStatus->GetSourcePar();
    TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*> (srcpar);

    ResetSourceWidgets(srcpar->GetName(), srcpar->GetTimeout(),
                        mbspar ? mbspar->GetStartEvent() : 0,
                        mbspar ? mbspar->GetStopEvent() : 0,
                        mbspar ? mbspar->GetEventInterval() : 0);
    FileNameInput->setEnabled(false); // JAM move out from ResetSourceWidgets to avoid conflict with hotstart

    switch(srcpar->GetID()) {
       case GO4EV_FILE: {
          SetFileSource();
          break;
       }
       case GO4EV_MBS_FILE: {
          TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(srcpar);
          SetMbsFileSource(filpar ? filpar->GetTagName() : "");
          break;
       }
       case GO4EV_MBS_STREAM: {
          SetMbsStreamSource();
          break;
       }
       case GO4EV_MBS_TRANSPORT: {
          SetMbsTransportSource();
          break;
       }
       case GO4EV_MBS_EVENTSERVER: {
          SetMbsEventServerSource();
          break;
       }
       case GO4EV_MBS_REVSERV: {
          TGo4RevServParameter* revpar = dynamic_cast<TGo4RevServParameter*>(srcpar);
          SetMbsRevServSource(revpar ? revpar->GetPort() : 0);
          break;
       }
       case GO4EV_MBS_RANDOM: {
          SetRandomSource();
          break;
       }
       case GO4EV_USER: {
          TGo4UserSourceParameter* usrpar = dynamic_cast<TGo4UserSourceParameter*>(srcpar);
          SetUserSource(usrpar ? usrpar->GetPort() : 0,
                        usrpar ? usrpar->GetExpression() : "");
          break;
       }
       default:
          break;

    } // SourcePar->GetID()



    //----------------------------------------------------//
    //Store

    TGo4EventStoreParameter* StorePar = fStepStatus->GetStorePar();

    if (StorePar!=0)
      switch(StorePar->GetID()) {
         case GO4EV_FILE: {
            TGo4FileStoreParameter* fstor = dynamic_cast<TGo4FileStoreParameter*>(StorePar);
            SetFileStore(fstor->GetName(),
                         fstor->IsOverwriteMode(),
                         fstor->GetBufsize(),
                         fstor->GetSplitlevel(),
                         fstor->GetCompression());
            break;
         }
         case GO4EV_BACK: {
            TGo4BackStoreParameter* bstor = dynamic_cast<TGo4BackStoreParameter*>(StorePar);
            SetBackStore(bstor->GetName(),
                         bstor->GetBufsize(),
                         bstor->GetSplitlevel());
            break;
         }
         default:
            break;
      } // switch StorePar->GetID()

    //----------------------------------------------------//
    // enable or disable processing:

    SetStepControl(fStepStatus->IsProcessEnabled(),
                   fStepStatus->IsSourceEnabled(),
                   fStepStatus->IsStoreEnabled());
}

void TGo4ConfigStep::SourceComboHighlighted(int k)
{
	//cout <<"TGo4ConfigStep::SourceComboHighlighted with k:"<<k << endl;
   SpinBoxPortNumber->setShown(false);
   TextLabelPortNumber->setShown(false);
   LineEditArgs->setShown(false);
   TextLabelArgs->setShown(false);
   LineEditTagfile->setShown(false);
   TextLabelTagfile->setShown(false);

   SpinBoxStartEvent->setEnabled(false);
   SpinBoxStopEvent->setEnabled(false);
   SpinBoxInterEvent->setEnabled(false);
   SpinBoxTimeout->setEnabled(false);
   FileNameInput->setEnabled(false);
   MbsMonitorBtn->setEnabled(false);

   // evaluate previous source if existing:
   Bool_t wasfilesource=kFALSE;
   TGo4EventSourceParameter* srcpar = fStepStatus->GetSourcePar();
   TGo4MbsFileParameter* mbsfilpar=dynamic_cast<TGo4MbsFileParameter*>(srcpar);
   TGo4FileSourceParameter* rootfilpar=dynamic_cast<TGo4FileSourceParameter*>(srcpar);
   TGo4UserSourceParameter* usersrc=dynamic_cast<TGo4UserSourceParameter*>(srcpar);
   TGo4MbsRandomParameter* randsrc=dynamic_cast<TGo4MbsRandomParameter*>(srcpar);
   if(mbsfilpar!=0 || rootfilpar!=0) wasfilesource=kTRUE;


   switch (k) {
      case 0: {            // root file with one tree
         TGo4FileSourceParameter newpar(SourceNameEdit->text().toAscii().constData());
         fStepStatus->SetSourcePar(&newpar);
         FileNameInput->setEnabled(true);
         if(rootfilpar==0) SourceNameEdit->clear();
         break;
      }
      case 1: {    // mbs listmode file (input only)
         TGo4MbsFileParameter newpar(SourceNameEdit->text().toAscii().constData());
         fStepStatus->SetSourcePar(&newpar);
         bool islml = SourceNameEdit->text().contains(TGo4MbsFile__fgcFILELISTSUF);
         // case of *.lml file: disable start/stop event fields
         LineEditTagfile->setShown(!islml);
         TextLabelTagfile->setShown(!islml);

         SpinBoxStartEvent->setEnabled(!islml);
         SpinBoxStopEvent->setEnabled(!islml);
         SpinBoxInterEvent->setEnabled(!islml);
         FileNameInput->setEnabled(true);
         if(mbsfilpar==0) SourceNameEdit->clear();
         break;
      }
      case 2: {  // mbs stream server (input only)
         TGo4MbsStreamParameter newpar(SourceNameEdit->text().toAscii().constData());
         newpar.SetPort(SpinBoxPortNumber->value());
         fStepStatus->SetSourcePar(&newpar);
         SpinBoxTimeout->setEnabled(true);
         TextLabelPortNumber->setShown(true);
         SpinBoxPortNumber->setShown(true);
         MbsMonitorBtn->setEnabled(true);
         SpinBoxStartEvent->setEnabled(true);
         SpinBoxStopEvent->setEnabled(true);
         SpinBoxInterEvent->setEnabled(true);
         if(wasfilesource || randsrc!=0 ) SourceNameEdit->clear();
         break;
      }
      case 3: {       // mbs transport server (input only)
         TGo4MbsTransportParameter newpar(SourceNameEdit->text().toAscii().constData());
         newpar.SetPort(SpinBoxPortNumber->value());
         fStepStatus->SetSourcePar(&newpar);
         SpinBoxTimeout->setEnabled(true);
         TextLabelPortNumber->setShown(true);
         SpinBoxPortNumber->setShown(true);
         MbsMonitorBtn->setEnabled(true);
         SpinBoxStartEvent->setEnabled(true);
         SpinBoxStopEvent->setEnabled(true);
         SpinBoxInterEvent->setEnabled(true);
         if(wasfilesource || randsrc!=0) SourceNameEdit->clear();
         break;
      }
      case 4: {     // mbs event server  (input only)
         TGo4MbsEventServerParameter newpar(SourceNameEdit->text().toAscii().constData());
         fStepStatus->SetSourcePar(&newpar);
         SpinBoxTimeout->setEnabled(true);
         SpinBoxPortNumber->setShown(true);
         TextLabelPortNumber->setShown(true);
         MbsMonitorBtn->setEnabled(true);
         SpinBoxStartEvent->setEnabled(true);
         SpinBoxStopEvent->setEnabled(true);
         SpinBoxInterEvent->setEnabled(true);
         if(wasfilesource || randsrc!=0) SourceNameEdit->clear();
         break;
      }
      case 5: {     //    rev serv
         TGo4RevServParameter newpar(SourceNameEdit->text().toAscii().constData());
         newpar.SetPort(SpinBoxPortNumber->value());
         fStepStatus->SetSourcePar(&newpar);
         SpinBoxPortNumber->setShown(true);
         TextLabelPortNumber->setShown(true);
         SpinBoxStartEvent->setEnabled(true);
         SpinBoxStopEvent->setEnabled(true);
         SpinBoxInterEvent->setEnabled(true);
         if(wasfilesource || randsrc!=0) SourceNameEdit->clear();
         break;
      }
      case 6: {     //    mbs random
         TGo4MbsRandomParameter newpar(SourceNameEdit->text().toAscii().constData());
         fStepStatus->SetSourcePar(&newpar);
         SourceNameEdit->setText("RandomEvents");
         break;
      }
      case 7: {     // user source
         TGo4UserSourceParameter newpar(SourceNameEdit->text().toAscii().constData());
         newpar.SetPort(SpinBoxPortNumber->value());
         newpar.SetExpression(LineEditArgs->text().trimmed().toAscii().constData());
         fStepStatus->SetSourcePar(&newpar);
         SpinBoxPortNumber->setShown(true);
         TextLabelPortNumber->setShown(true);
         LineEditArgs->setShown(true);
         TextLabelArgs->setShown(true);
         SpinBoxTimeout->setEnabled(true);
         FileNameInput->setEnabled(true);
         if(usersrc==0) SourceNameEdit->clear();
         break;
      }
   }
   adjustSize();
   parentWidget()->adjustSize();
   parentWidget()->parentWidget()->adjustSize();
   parentWidget()->parentWidget()->parentWidget()->adjustSize();
}

void TGo4ConfigStep::StoreComboHighlighted(int k)
{
   if(k==0) {
      StoreNameEdit->setDisabled(FALSE);
      TGo4FileStoreParameter* newpar1 = new TGo4FileStoreParameter(StoreNameEdit->text().toAscii().constData());
      fStepStatus->SetStorePar(newpar1);
      delete newpar1;
      CompLevel->setDisabled(FALSE);
      StoreOverwriteMode->setDisabled(FALSE);
      FileNameOutput->setDisabled(FALSE);
   } else
   if(k==1) {
      StoreNameEdit->setDisabled(TRUE);
      TGo4BackStoreParameter *newpar3 = new TGo4BackStoreParameter(GetBackStoreName().toAscii().constData());
      fStepStatus->SetStorePar(newpar3);
      delete newpar3;
      CompLevel->setDisabled(TRUE);
      StoreOverwriteMode->setDisabled(TRUE);
      FileNameOutput->setDisabled(TRUE);
    }
}


void TGo4ConfigStep::OutputFileDialog()
{
    QFileDialog fd( this, "Select file name for step output",
          fxPanel->GetStorePath(), "Go4FileStore  (*.root)");
    fd.setFileMode( QFileDialog::AnyFile);
    if ( fd.exec() != QDialog::Accepted ) return;

    QStringList flst = fd.selectedFiles();
    if (flst.isEmpty()) return;

    QString fileName = flst[0];
    fxPanel->SetStorePath(fd.directory().path());
    if(!fileName.endsWith(".root")) fileName.append(".root");
    StoreNameEdit->setText(fileName);
}

void TGo4ConfigStep::StoreBufferSize( int t )
{
   if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())){
       TGo4FileStoreParameter *StorePar=(TGo4FileStoreParameter *)fStepStatus->GetStorePar();
       StorePar->SetBufsize(1000*t);
   } else
   if (fStepStatus->GetStorePar()->InheritsFrom(TGo4BackStoreParameter::Class())){
       TGo4BackStoreParameter * StorePar = (TGo4BackStoreParameter *)fStepStatus->GetStorePar();
       StorePar->SetBufsize(1000*t);
   }
}

void TGo4ConfigStep::StoreSplitLevel( int t)
{
   if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())) {
      TGo4FileStoreParameter *StorePar=(TGo4FileStoreParameter *)fStepStatus->GetStorePar();
      StorePar->SetSplitlevel(t);
   } else
   if (fStepStatus->GetStorePar()->InheritsFrom(TGo4BackStoreParameter::Class())) {
       TGo4BackStoreParameter * StorePar = (TGo4BackStoreParameter *)fStepStatus->GetStorePar();
       StorePar->SetSplitlevel(t);
   }
}

void TGo4ConfigStep::StoreCompLevel( int t)
{
   if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())) {
       TGo4FileStoreParameter *StorePar=(TGo4FileStoreParameter *)fStepStatus->GetStorePar();
       StorePar->SetCompression(t);
   }
}

void TGo4ConfigStep::StoreOverWrite( bool overwrite)
{
   if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())){
        TGo4FileStoreParameter *StorePar=dynamic_cast <TGo4FileStoreParameter *> (fStepStatus->GetStorePar());
         if(overwrite)StorePar->SetOverwriteMode(kTRUE);
         else StorePar->SetOverwriteMode(kFALSE);
    }
}


void TGo4ConfigStep::InputTagfile( const QString & tag )
{
   TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
   int ParId=SourcePar->GetID();
   if (ParId!=GO4EV_MBS_FILE) return;
                // tagfile only for mbs lmd file
   TGo4MbsFileParameter* filpar = dynamic_cast<TGo4MbsFileParameter*>(SourcePar);

   if (filpar==0) return;

   if(tag.isEmpty())
      filpar->SetTagName(TGo4MbsFile__fgcNOTAGFILE);
   else
      filpar->SetTagName(tag.trimmed().toAscii().constData());
}

void TGo4ConfigStep::StoreStartEvent( int num )
{
   // only for mbs sources
   TGo4MbsSourceParameter* par =
      dynamic_cast<TGo4MbsFileParameter*>(fStepStatus->GetSourcePar());
   if (par!=0) par->SetStartEvent(num);
}

void TGo4ConfigStep::StoreStopEvent( int num )
{
  // only for mbs sources
  TGo4MbsSourceParameter* par =
     dynamic_cast<TGo4MbsFileParameter*>(fStepStatus->GetSourcePar());
  if (par!=0) par->SetStopEvent(num);
}

void TGo4ConfigStep::StoreEventInterval( int num )
{
  // only for mbs sources
  TGo4MbsSourceParameter* par =
     dynamic_cast<TGo4MbsFileParameter*>(fStepStatus->GetSourcePar());
  if (par!=0) par->SetEventInterval(num);
}

void TGo4ConfigStep::StoreTimeout( int tim )
{
   TGo4EventSourceParameter* sourcepar = fStepStatus->GetSourcePar();
   if (sourcepar!=0)
      sourcepar->SetTimeout(tim);
}

void TGo4ConfigStep::InputFileDialog()
{
   QString filters;
   bool mbsfilemode = false;
   if(fStepStatus!=0) {
       TGo4EventSourceParameter* sourcepar = fStepStatus->GetSourcePar();
       if(sourcepar->InheritsFrom(TGo4FileSourceParameter::Class()))
          filters = "Go4FileSource  (*.root)";
       else
       if (sourcepar->InheritsFrom(TGo4MbsFileParameter::Class())) {
           mbsfilemode=true;
           filters="Go4MbsFile (*";
           filters+=TGo4MbsFile__fgcLMDSUF;
           filters+=" *";
           filters+=QString(TGo4MbsFile__fgcLMDSUF).toUpper();
           filters+=");;Go4 list mode list (*";
           filters+=TGo4MbsFile__fgcFILELISTSUF;
           filters+=")";

       }
       else
       if (sourcepar->InheritsFrom(TGo4UserSourceParameter::Class())) {
          filters = "all files  (*.*)";
       }
       else
          cout <<"Unknown sourcepar " <<sourcepar->ClassName() << endl;
   }

    //cout <<"FFFFFFFF chose filter-"<< filters.toAscii().constData() <<"-" <<endl;
    QFileDialog fd( this, "Select file name for step input",
                          fxPanel->GetSourcePath(), filters);
    fd.setFileMode(QFileDialog::ExistingFile);

    if ( fd.exec() != QDialog::Accepted ) return;

    QStringList flst = fd.selectedFiles();
    if (flst.isEmpty()) return;

    QString fileName = flst[0];
    fxPanel->SetSourcePath(fd.directory().path());
    SourceNameEdit->setText(fileName);
    if(mbsfilemode) {
       bool islml = fd.selectedNameFilter().contains(TGo4MbsFile__fgcFILELISTSUF);

       LineEditTagfile->setShown(!islml);
       TextLabelTagfile->setShown(!islml);
       SpinBoxStartEvent->setEnabled(!islml);
       SpinBoxStopEvent->setEnabled(!islml);
       SpinBoxInterEvent->setEnabled(!islml);
    }
}

QString TGo4ConfigStep::GetStepName()
{
   return fStepStatus->GetName();
}

QString TGo4ConfigStep::GetTabTitle()
{
   QString res = fStepStatus->GetName();
   if (fStepNumber>=0) {
      res += " ";
      res += EnableStepBox->isChecked() ? "x" : "o";
      res += EnableSourceBox->isChecked() ? "x" : "o";
      res += EnableStoreBox->isChecked() ? "x" : "o";
   }
   return res;
}

void TGo4ConfigStep::SetStepControl(bool process, bool source, bool store)
{
   EnableStepBox->setChecked(process);
   EnableSourceBox->setChecked(source);
   EnableStoreBox->setChecked(store);

   StepStateChanged(0);
}

void TGo4ConfigStep::GetStepControl(bool& process, bool& source, bool& store)
{
   process = EnableStepBox->isChecked();
   source = EnableSourceBox->isChecked();
   store = EnableStoreBox->isChecked();
}

void TGo4ConfigStep::ResetSourceWidgets(const QString& name,
                                        int timeout,
                                        int start, int stop, int interval)
{
	//cout <<"TGo4ConfigStep::ResetSourceWidgests for "<< name.toAscii().constData()<< endl;
    SourceNameEdit->setText(name);
    SpinBoxTimeout->setValue(timeout);
    SpinBoxStartEvent->setValue(start);
    SpinBoxStopEvent->setValue(stop);
    SpinBoxInterEvent->setValue(interval);

    SpinBoxPortNumber->setShown(false);
    TextLabelPortNumber->setShown(false);
    LineEditArgs->setShown(false);
    TextLabelArgs->setShown(false);
    LineEditTagfile->setShown(false);
    TextLabelTagfile->setShown(false);

    SpinBoxStartEvent->setEnabled(false);
    SpinBoxStopEvent->setEnabled(false);
    SpinBoxInterEvent->setEnabled(false);
    SpinBoxTimeout->setEnabled(false);
    //FileNameInput->setEnabled(false);
}

void TGo4ConfigStep::SetFileSource()
{
   EventSourceCombo->setCurrentIndex(0);
   FileNameInput->setEnabled(true);
   SourceComboHighlighted(0);
}

void TGo4ConfigStep::SetMbsFileSource(QString TagFile)
{
   EventSourceCombo->setCurrentIndex(1);
   LineEditTagfile->setShown(true);
   TextLabelTagfile->setShown(true);

   FileNameInput->setEnabled(true);
   if(!TagFile.contains(TGo4MbsFile__fgcNOTAGFILE))
      LineEditTagfile->setText(TagFile);
   else
      LineEditTagfile->setText("");
   SourceComboHighlighted(1);
}

void TGo4ConfigStep::SetMbsStreamSource()
{
   EventSourceCombo->setCurrentIndex(2);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(2);
}

void TGo4ConfigStep::SetMbsTransportSource()
{
   EventSourceCombo->setCurrentIndex(3);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(3);
}

void TGo4ConfigStep::SetMbsEventServerSource()
{
   EventSourceCombo->setCurrentIndex(4);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(4);
}

void TGo4ConfigStep::SetMbsRevServSource(int port)
{
   EventSourceCombo->setCurrentIndex(5);
   SpinBoxPortNumber->setValue(port);
   SpinBoxPortNumber->setShown(true);
   TextLabelPortNumber->setShown(true);
   LineEditArgs->setShown(true);
   TextLabelArgs->setShown(true);
   SourceComboHighlighted(5);
}

void TGo4ConfigStep::SetRandomSource()
{
   EventSourceCombo->setCurrentIndex(6);
   SourceComboHighlighted(6);
}

void TGo4ConfigStep::SetUserSource(int port, QString expr)
{
   EventSourceCombo->setCurrentIndex(7);
   SpinBoxPortNumber->setValue(port);
   SpinBoxPortNumber->setShown(true);
   TextLabelPortNumber->setShown(true);
   LineEditArgs->setText(expr);
   LineEditArgs->setShown(true);
   TextLabelArgs->setShown(true);
   SpinBoxTimeout->setEnabled(true);
   FileNameInput->setEnabled(true);
   SourceComboHighlighted(7);
}

int TGo4ConfigStep::GetSourceSetup(QString& name, int& timeout, int& start, int& stop, int& interval)
{
   name = SourceNameEdit->text();
   timeout = SpinBoxTimeout->value();
   start = SpinBoxStartEvent->value();
   stop = SpinBoxStopEvent->value();
   interval = SpinBoxInterEvent->value();
   return EventSourceCombo->currentIndex();
}

void TGo4ConfigStep::GetMbsFileSource(QString& TagFile)
{
   TagFile = LineEditTagfile->text();
//   if (TagFile.length()==0) TagFile = TGo4MbsFile__fgcNOTAGFILE;
}

void TGo4ConfigStep::GetMbsRevServSource(int& port)
{
   port = SpinBoxPortNumber->value();
}

void TGo4ConfigStep::GetUserSource(int& port, QString& expr)
{
   port = SpinBoxPortNumber->value();
   expr = LineEditArgs->text();
}

void TGo4ConfigStep::SetFileStore(QString name, bool overwrite, int bufsize, int splitlevel, int compression)
{
   StoreNameEdit->setEnabled(TRUE);
   StoreNameEdit->setText(name);
   OutputCombo->setCurrentIndex(0);
   StoreOverwriteMode->setChecked(overwrite);
   StoreOverwriteMode->setEnabled(TRUE);
   BufferSize->setValue(bufsize/1000);
   SplitLevel->setValue(splitlevel);
   CompLevel->setValue(compression);
   CompLevel->setEnabled(TRUE);
   FileNameOutput->setEnabled(TRUE);
   StoreComboHighlighted(0);
}

void TGo4ConfigStep::SetBackStore(QString name, int bufsize, int splitlevel)
{
   StoreNameEdit->setEnabled(FALSE);
   StoreNameEdit->setText(GetBackStoreName());
   OutputCombo->setCurrentIndex(1);
   BufferSize->setValue(bufsize/1000);
   SplitLevel->setValue(splitlevel);
   StoreOverwriteMode->setEnabled(FALSE);
   CompLevel->setEnabled(FALSE);
   FileNameOutput->setEnabled(FALSE);
   StoreComboHighlighted(1);
}

QString TGo4ConfigStep::GetBackStoreName()
{
   QString name = "BackStoreTree_";
   name += GetStepName();
   return name;
}

int TGo4ConfigStep::GetStoreSetup(QString& name)
{
   int typ = OutputCombo->currentIndex();
   if (typ==1) name = GetBackStoreName();
          else name = StoreNameEdit->text();
   return typ;
}

void TGo4ConfigStep::GetFileStore(bool& overwrite, int& bufsize, int& splitlevel, int& compression)
{
   overwrite = StoreOverwriteMode->isChecked();
   bufsize = BufferSize->value() * 1000;
   splitlevel = SplitLevel->value();
   compression = CompLevel->value();
}

void TGo4ConfigStep::GetBackStore(int& bufsize, int& splitlevel)
{
   bufsize = BufferSize->value() * 1000;
   splitlevel = SplitLevel->value();
}

void TGo4ConfigStep::MbsMonitorBtn_clicked()
{
   if (fxPanel!=0)
     fxPanel->DisplayMbsMonitor(SourceNameEdit->text());
}
