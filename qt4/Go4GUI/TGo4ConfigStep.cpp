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

enum SourceComboKinds {
   kind_RootFile = 0,
   kind_MbsFile = 1,
   kind_MbsStream = 2,
   kind_MbsTransport = 3,
   kind_MbsEvent = 4,
   kind_MbsREvent = 5,
   kind_MbsRandom = 6,
   kind_UserSource = 7
};


TGo4ConfigStep::TGo4ConfigStep( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, fl )
{
   setObjectName( name ? name : "Go4ConfigStep");

   setupUi(this);

   fxPanel = 0;
   fStepStatus = 0;

   SpinBoxPortNumber->setShown(false);
   TextLabelPortNumber->setShown(false);
   SpinBoxRetryNumber->setShown(false);
   TextLabelRetryNumber->setShown(false);

   LineEditArgs->setShown(false);
   TextLabelArgs->setShown(false);
   LineEditTagfile->setShown(false);
   TextLabelTagfile->setShown(false);

   for (int n=0;n<ParsSize;n++)
      fPars[n]=0;

   fLastSrcKind = -1;
   fBlocked = 0;
}

TGo4ConfigStep::~TGo4ConfigStep()
{
   for (int n=0;n<ParsSize;n++) {
      delete fPars[n];
      fPars[n] = 0;
   }
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
   if (fBlocked) return;

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

void TGo4ConfigStep::RetryCntChanged(int cnt)
{
   if (fBlocked) return;

   TGo4EventSourceParameter* SourcePar = fStepStatus->GetSourcePar();

   switch (SourcePar->GetID()) {

      case GO4EV_MBS_TRANSPORT:
      case GO4EV_MBS_STREAM:
      case GO4EV_MBS_EVENTSERVER:
      case GO4EV_MBS_REVSERV: {
         TGo4MbsSourceParameter* par = dynamic_cast<TGo4MbsSourceParameter*>(SourcePar);
         if (par!=0)
            par->SetRetryCnt(cnt);
         break;
      }

      default: {
         // do nothing
         break;
      }
   }
}

void TGo4ConfigStep::InputSourceText(const QString& name)
{
   if (fBlocked) return;

   if (fStepStatus && fStepStatus->GetSourcePar())
      fStepStatus->GetSourcePar()->SetName(name.trimmed().toAscii().constData());

   if (EventSourceCombo->currentIndex() == kind_MbsFile) {
      bool islml = name.contains(TGo4MbsFile__fgcFILELISTSUF);
      LineEditTagfile->setShown(!islml);
      TextLabelTagfile->setShown(!islml);

      SpinBoxStartEvent->setEnabled(!islml);
      SpinBoxStopEvent->setEnabled(!islml);
      SpinBoxInterEvent->setEnabled(!islml);
   }
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

void TGo4ConfigStep::SetStepStatus(TGo4AnalysisConfiguration* panel, TGo4AnalysisStepStatus* StepStatus, int number)
{
    if (StepStatus==0) return;
    //cout <<"TGo4ConfigStep::SetStepStatus for "<< StepStatus->GetName()<< endl;

    fxPanel = panel;
    fStepStatus = StepStatus;
    fStepNumber = number;
    fLastSrcKind = -1; // indicate that status (and its parameter) is changed

    //----------------------------------------------------//
    //Source:

    /*
    Qt use following order to display the source parameter:

    0)  root file with one tree                 TGo4FileSourceParameter
    1)  mbs listmode file (input only)          TGo4MbsFileParameter
    2)  mbs stream server (input only)          TGo4MbsStreamParameter
    3)  mbs transport server (input only)       TGo4MbsTransportParameter
    4)  mbs event server  (input only)          TGo4MbsEventServerParameter
    5)  rev serv                                TGo4RevServParameter
    6)  mbs random generator                    TGo4MbsRandomParameter
    7)  user source                             TGo4UserSourceParameter

    */

    int srckind = CurrentSourceKind();
    fBlocked++;
    EventSourceCombo->setCurrentIndex(srckind);
    fBlocked--;


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


int TGo4ConfigStep::CurrentSourceKind()
{
   if ((fStepStatus==0) || (fStepStatus->GetSourcePar()==0)) return kind_MbsFile;

   switch(fStepStatus->GetSourcePar()->GetID()) {
      case GO4EV_FILE: return kind_RootFile;
      case GO4EV_MBS_FILE: return kind_MbsFile;
      case GO4EV_MBS_STREAM: return kind_MbsStream;
      case GO4EV_MBS_TRANSPORT: return kind_MbsTransport;
      case GO4EV_MBS_EVENTSERVER: return kind_MbsEvent;
      case GO4EV_MBS_REVSERV: return kind_MbsREvent;
      case GO4EV_MBS_RANDOM: return kind_MbsRandom;
      case GO4EV_USER: return kind_UserSource;
   } // SourcePar->GetID()

   return kind_MbsFile;
}

void TGo4ConfigStep::ChangeSourceParameter(int kind)
{
   if (fStepStatus==0) return;

   TGo4EventSourceParameter* srcpar = fStepStatus->TakeSourcePar();
   bool delsrcpar(true);
   TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*> (srcpar);

   if (fLastSrcKind >= 0) {
      if (fPars[fLastSrcKind]!=0) delete fPars[fLastSrcKind];
      fPars[fLastSrcKind] = srcpar;
      delsrcpar = false;
   }

   if (fPars[kind]) {
      fStepStatus->SetSourcePar(fPars[kind]);
      fLastSrcKind = kind;
      if (delsrcpar) delete srcpar;
      return;
   }

   TString filename, hostname;
   Int_t port(0);

   switch(srcpar->GetID()) {
      case GO4EV_FILE:
      case GO4EV_MBS_FILE:
         filename = srcpar->GetName();
         break;

      case GO4EV_MBS_STREAM:
      case GO4EV_MBS_TRANSPORT:
      case GO4EV_MBS_EVENTSERVER:
      case GO4EV_MBS_REVSERV:
         hostname = srcpar->GetName();
         port = mbspar ? mbspar->GetPort() : 0;
         break;

      case GO4EV_MBS_RANDOM:
         break;
      case GO4EV_USER:
         break;
      default:
         break;
   }

   TGo4EventSourceParameter* newpar = 0;

   switch (kind) {
      case kind_RootFile:
         newpar = new TGo4FileSourceParameter(filename);
         break;

      case kind_MbsFile:
         newpar = new TGo4MbsFileParameter(filename);
         break;

      case kind_MbsStream:
         newpar = new TGo4MbsStreamParameter(hostname);
         break;

      case kind_MbsTransport:
         newpar = new TGo4MbsTransportParameter(hostname);
         break;

      case kind_MbsEvent:
         newpar = new TGo4MbsEventServerParameter(hostname);
         break;

      case kind_MbsREvent:
         newpar = new TGo4RevServParameter(hostname);
         break;

      case kind_MbsRandom:
         newpar = new TGo4MbsRandomParameter("random");
         break;

      case kind_UserSource:
         newpar = new TGo4UserSourceParameter(filename,"",port);
         break;
   }

   TGo4MbsSourceParameter* newmbspar = dynamic_cast<TGo4MbsSourceParameter*> (newpar);

   if (newmbspar) {
      newmbspar->SetPort(port);

      if (mbspar) {
         newmbspar->SetStartEvent(mbspar->GetStartEvent());
         newmbspar->SetStopEvent(mbspar->GetStopEvent());
         newmbspar->SetEventInterval(mbspar->GetEventInterval());
      }
   }

   newpar->SetTimeout(srcpar->GetTimeout());

   fPars[kind] = newpar;

   // here parameter will be cloned
   fStepStatus->SetSourcePar(fPars[kind]);

   fLastSrcKind = kind;

   if (delsrcpar) delete srcpar;
}

void TGo4ConfigStep::SetSourceWidgets( const QString & name, int timeout)
{
   SourceNameEdit->setText(name);
   SpinBoxTimeout->setValue(timeout);
}

void TGo4ConfigStep::SetMbsSourceWidgets(int start, int stop, int interval, int port)
{
   SpinBoxStartEvent->setValue(start);
   SpinBoxStopEvent->setValue(stop);
   SpinBoxInterEvent->setValue(interval);
   SpinBoxPortNumber->setValue(port);
   SpinBoxRetryNumber->setValue(0);

}

void TGo4ConfigStep::SourceComboHighlighted(int kind)
{
   // FIXME: do it only interactively
   if (fBlocked == 0) ChangeSourceParameter(kind);

   fBlocked++;

   // first disable everything which can be disabled

   SpinBoxPortNumber->setShown(false);
   TextLabelPortNumber->setShown(false);
   SpinBoxRetryNumber->setShown(false);
   TextLabelRetryNumber->setShown(false);
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

   // evaluate previous source if existing

   TGo4EventSourceParameter* srcpar = fStepStatus->GetSourcePar();

   SourceNameEdit->setText(srcpar->GetName());
   SpinBoxTimeout->setValue(srcpar->GetTimeout());

   TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*> (srcpar);

   if (mbspar) {
      SpinBoxStartEvent->setValue(mbspar->GetStartEvent());
      SpinBoxStopEvent->setValue(mbspar->GetStopEvent());
      SpinBoxInterEvent->setValue(mbspar->GetEventInterval());
      SpinBoxPortNumber->setValue(mbspar->GetPort());
      SpinBoxRetryNumber->setValue(mbspar->GetRetryCnt());
   }

   TGo4MbsFileParameter* mbsfilpar = dynamic_cast<TGo4MbsFileParameter*>(srcpar);
   TGo4UserSourceParameter* userpar = dynamic_cast<TGo4UserSourceParameter*>(srcpar);

   switch (kind) {
      case kind_RootFile:            // root file with one tree
         FileNameInput->setEnabled(true);
         break;
      case kind_MbsFile: {            // mbs listmode file (input only)
         // in this special case spin boxes will be enabled/disabled when file name is changed
         FileNameInput->setEnabled(true);

         LineEditTagfile->setShown(true);
         TextLabelTagfile->setShown(true);

         QString tagfile = mbsfilpar->GetTagName();

         if(!tagfile.contains(TGo4MbsFile__fgcNOTAGFILE))
            LineEditTagfile->setText(tagfile);
         else
            LineEditTagfile->setText("");

         break;
      }
      case kind_MbsStream:       // mbs stream server (input only)
      case kind_MbsTransport:    // mbs transport server (input only)
      case kind_MbsEvent:        // mbs event server  (input only)
      case kind_MbsREvent:       // rev serv
         SpinBoxTimeout->setEnabled(true);
         TextLabelPortNumber->setShown(true);
         SpinBoxPortNumber->setShown(true);
         TextLabelRetryNumber->setShown(true);
         SpinBoxRetryNumber->setShown(true);
         MbsMonitorBtn->setEnabled(kind != kind_MbsREvent);
         SpinBoxStartEvent->setEnabled(true);
         SpinBoxStopEvent->setEnabled(true);
         SpinBoxInterEvent->setEnabled(true);
         break;

      case kind_MbsRandom:       //    mbs random
         break;

      case kind_UserSource:      // user source
         SpinBoxPortNumber->setValue(userpar->GetPort());
         LineEditArgs->setText(userpar->GetExpression());
         SpinBoxPortNumber->setShown(true);
         TextLabelPortNumber->setShown(true);
         LineEditArgs->setShown(true);
         TextLabelArgs->setShown(true);
         SpinBoxTimeout->setEnabled(true);
         FileNameInput->setEnabled(true);
         break;
   }

   fBlocked--;

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
   if (fBlocked) return;
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

void TGo4ConfigStep::ChangeStartEvent( int num )
{
   if (fBlocked) return;

   // only for mbs sources
   TGo4MbsSourceParameter* par =
      dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
   if (par!=0) par->SetStartEvent(num);
}

void TGo4ConfigStep::ChangeStopEvent( int num )
{
   if (fBlocked) return;

  // only for mbs sources
  TGo4MbsSourceParameter* par =
     dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
  if (par!=0) par->SetStopEvent(num);
}

void TGo4ConfigStep::ChangeEventInterval( int num )
{
   if (fBlocked) return;

   // only for mbs sources
   TGo4MbsSourceParameter* par =
      dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
   if (par!=0) par->SetEventInterval(num);
}

void TGo4ConfigStep::ChangeSourceTimeout( int tim )
{
   if (fBlocked) return;

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

void TGo4ConfigStep::SetFileSource()
{
   EventSourceCombo->setCurrentIndex(kind_RootFile);
}

void TGo4ConfigStep::SetMbsFileSource(const QString& TagFile)
{
   EventSourceCombo->setCurrentIndex(kind_MbsFile);

   if(!TagFile.contains(TGo4MbsFile__fgcNOTAGFILE))
      LineEditTagfile->setText(TagFile);
   else
      LineEditTagfile->setText("");
}

void TGo4ConfigStep::SetMbsStreamSource()
{
   EventSourceCombo->setCurrentIndex(kind_MbsStream);
}

void TGo4ConfigStep::SetMbsTransportSource()
{
   EventSourceCombo->setCurrentIndex(kind_MbsTransport);
}

void TGo4ConfigStep::SetMbsEventServerSource()
{
   EventSourceCombo->setCurrentIndex(kind_MbsEvent);
}

void TGo4ConfigStep::SetMbsRevServSource(int port)
{
   EventSourceCombo->setCurrentIndex(kind_MbsREvent);

   SpinBoxPortNumber->setValue(port>0 ? port : 0);
}

void TGo4ConfigStep::SetMbsPort(int port)
{
   SpinBoxPortNumber->setValue(port > 0 ? port : 0);
}

void TGo4ConfigStep::SetMbsRetryCnt(int retry)
{
   SpinBoxRetryNumber->setValue(retry > 0 ? retry : 0);
}


void TGo4ConfigStep::SetRandomSource()
{
   EventSourceCombo->setCurrentIndex(kind_MbsRandom);
}

void TGo4ConfigStep::SetUserSource(int port, QString expr)
{
   EventSourceCombo->setCurrentIndex(kind_UserSource);

   SpinBoxPortNumber->setValue(port);
   LineEditArgs->setText(expr);
}

int TGo4ConfigStep::GetSourceSetup(QString& name, int& timeout, int& start, int& stop, int& interval, int& nport, int & nretry)
{
   name = SourceNameEdit->text();
   timeout = SpinBoxTimeout->value();
   start = SpinBoxStartEvent->value();
   stop = SpinBoxStopEvent->value();
   interval = SpinBoxInterEvent->value();
   nport = -1;
   nretry = 0;
   switch (fStepStatus->GetSourcePar()->GetID()) {
      case GO4EV_MBS_TRANSPORT:
      case GO4EV_MBS_STREAM:
      case GO4EV_MBS_EVENTSERVER:
      case GO4EV_MBS_REVSERV: {
         TGo4MbsSourceParameter* par = dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
         if (par!=0) {
            nport = SpinBoxPortNumber->value();
            nretry = SpinBoxRetryNumber->value();
         }
         break;
      }
   }
   return EventSourceCombo->currentIndex();
}

void TGo4ConfigStep::GetMbsFileSource(QString& TagFile)
{
   TagFile = LineEditTagfile->text();
//   if (TagFile.length()==0) TagFile = TGo4MbsFile__fgcNOTAGFILE;
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
