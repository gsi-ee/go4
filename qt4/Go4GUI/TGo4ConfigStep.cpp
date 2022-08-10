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

#include "TGo4ConfigStep.h"

#include <iostream>

#include <QFileDialog>

#include "TGo4AnalysisStepStatus.h"
#include "TGo4EventStoreParameter.h"
#include "TGo4EventSourceParameter.h"
#include "TGo4UserSourceParameter.h"
#include "TGo4UserStoreParameter.h"
#include "TGo4RevServParameter.h"
#include "TGo4FileStoreParameter.h"
#include "TGo4BackStoreParameter.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4MbsFileParameter.h"
#include "TGo4MbsStreamParameter.h"
#include "TGo4MbsTransportParameter.h"
#include "TGo4MbsEventServerParameter.h"
#include "TGo4MbsRandomParameter.h"

#ifdef __GO4HDF5__
//#include "H5Cpp.h"
#include "TGo4HDF5StoreParameter.h"
#include "TGo4HDF5SourceParameter.h"
#endif

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
   kind_UserSource = 7,
   kind_HDF5 = 8
};


TGo4ConfigStep::TGo4ConfigStep( QWidget* parent, const char* name, Qt::WindowFlags fl )
    : QWidget( parent, fl )
{
   setObjectName( name ? name : "Go4ConfigStep");

   setupUi(this);

   QObject::connect(EnableStepBox, SIGNAL(stateChanged(int)), this, SLOT(StepStateChanged(int)));
   QObject::connect(EnableSourceBox, SIGNAL(stateChanged(int)), this, SLOT(InputStateChanged(int)));
   QObject::connect(EnableStoreBox, SIGNAL(stateChanged(int)), this, SLOT(OutputStateChanged(int)));
   QObject::connect(SourceNameEdit, &QLineEdit::textChanged, this, &TGo4ConfigStep::InputSourceText);
   QObject::connect(EventSourceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SourceComboHighlighted(int)));
   QObject::connect(OutputCombo, QOverload<int>::of(&QComboBox::activated), this, &TGo4ConfigStep::StoreComboHighlighted);
   QObject::connect(StoreNameEdit, &QLineEdit::textChanged, this, &TGo4ConfigStep::OutputNameText);
   QObject::connect(FileNameOutput, SIGNAL(clicked()), this, SLOT(OutputFileDialog()));
   QObject::connect(CompLevel, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::StoreCompLevel);
   QObject::connect(BufferSize, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::StoreBufferSize);
   QObject::connect(SplitLevel, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::StoreSplitLevel);
   QObject::connect(StoreOverwriteMode, &QCheckBox::toggled, this, &TGo4ConfigStep::StoreOverWrite);
   QObject::connect(LineEditArgs, &QLineEdit::textChanged, this, &TGo4ConfigStep::InputArguments);
   QObject::connect(LineEditTagfile, &QLineEdit::textChanged, this, &TGo4ConfigStep::InputTagfile);
   QObject::connect(SpinBoxStartEvent, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::ChangeStartEvent);
   QObject::connect(SpinBoxStopEvent, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::ChangeStopEvent);
   QObject::connect(FileNameBtn, SIGNAL(clicked()), this, SLOT(InputFileDialog()));
   QObject::connect(SpinBoxInterEvent, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::ChangeEventInterval);
   QObject::connect(SpinBoxTimeout, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::ChangeSourceTimeout);
   QObject::connect(ExtraBtn, SIGNAL(clicked()), this, SLOT(ExtraBtn_clicked()));
   QObject::connect(MbsMonitorBtn, SIGNAL(clicked()), this, SLOT(MbsMonitorBtn_clicked()));
   QObject::connect(SpinBoxPortNumber, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::InputPortChanged);
   QObject::connect(SpinBoxRetryNumber, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::RetryCntChanged);
   QObject::connect(TreeAutosave, (void (QSpinBox::*)(int)) &QSpinBox::valueChanged, this, &TGo4ConfigStep::StoreTreeAutoSave);

   fStepNumber=0;
   fxPanel = 0;
   fStepStatus = 0;

   SpinBoxPortNumber->setVisible(false);
   TextLabelPortNumber->setVisible(false);
   SpinBoxRetryNumber->setVisible(false);
   TextLabelRetryNumber->setVisible(false);
   SpinBoxTimeout->setVisible(false);
   TextLabelTimeout->setVisible(false);

   SpinBoxStartEvent->setVisible(false);
   SpinBoxStopEvent->setVisible(false);
   SpinBoxInterEvent->setVisible(false);

   LineEditArgs->setVisible(false);
   TextLabelArgs->setVisible(false);
   LineEditTagfile->setVisible(false);
   TextLabelTagfile->setVisible(false);

   for (int n=0;n<ParsSize;n++)
      fPars[n]=0;

   fLastSrcKind = -1;
   fBlocked = 0;
   fExtra = false;
   ExtraBtn->setText(fExtra ? "-" : "+");

#ifndef __GO4HDF5__
   OutputCombo->removeItem(3);
   EventSourceCombo->removeItem(8);
#endif

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
        TGo4UserSourceParameter* usrpar = dynamic_cast<TGo4UserSourceParameter*>(SourcePar);
        if (usrpar)
           usrpar->SetExpression(Arg.trimmed().toLatin1().constData());
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
         if (par)
            par->SetPort(port);
         break;
      }

      case GO4EV_USER: { // user defined source class
         TGo4UserSourceParameter* usrpar=dynamic_cast<TGo4UserSourceParameter*>(SourcePar);
         if (usrpar)
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
         if (par)
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
      fStepStatus->GetSourcePar()->SetName(name.trimmed().toLatin1().constData());

   if (EventSourceCombo->currentIndex() == kind_MbsFile) {
      bool islml = name.contains(TGo4MbsFile__fgcFILELISTSUF);
      LineEditTagfile->setVisible(!islml);
      TextLabelTagfile->setVisible(!islml);

      SpinBoxStartEvent->setEnabled(!islml);
      SpinBoxStopEvent->setEnabled(!islml);
      SpinBoxInterEvent->setEnabled(!islml);
   }
}

void TGo4ConfigStep::OutputStateChanged(int)
{
   bool stepon = EnableStepBox->isChecked();
   bool on = EnableStoreBox->isChecked();

   fStepStatus->SetStoreEnabled(on);
   StoreBox->setEnabled(stepon && on);
   StoreBox->setVisible(on);

   fxPanel->ChangeTabTitle(this, fStepNumber);

   adjustSize();
   parentWidget()->adjustSize();
   parentWidget()->parentWidget()->adjustSize();

   fxPanel->adjustSize();
   fxPanel->parentWidget()->adjustSize();
}

void TGo4ConfigStep::OutputNameText(const QString& Name)
{
   fStepStatus->GetStorePar()->SetName(Name.trimmed().toLatin1().constData());
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
    if (!StepStatus) return;
    //std::cout <<"TGo4ConfigStep::SetStepStatus for "<< StepStatus->GetName()<< std::endl;

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

    if (StorePar)
      switch(StorePar->GetID()) {
         case GO4EV_FILE: {
            TGo4FileStoreParameter* fstor = dynamic_cast<TGo4FileStoreParameter*>(StorePar);
            SetFileStore(fstor->GetName(),
                         fstor->IsOverwriteMode(),
                         fstor->GetBufsize(),
                         fstor->GetSplitlevel(),
                         fstor->GetCompression(),
                         fstor->GetAutosaveSize());
            break;
         }
         case GO4EV_BACK: {
            TGo4BackStoreParameter* bstor = dynamic_cast<TGo4BackStoreParameter*>(StorePar);
            SetBackStore(bstor->GetName(),
                         bstor->GetBufsize(),
                         bstor->GetSplitlevel());
            break;
         }
         case GO4EV_USER: {
            TGo4UserStoreParameter *ustor = dynamic_cast<TGo4UserStoreParameter*>(StorePar);
            SetUserStore(ustor->GetName());
            break;
         }

#ifdef __GO4HDF5__
         case GO4EV_HDF5: {
           TGo4HDF5StoreParameter* hstor = dynamic_cast<TGo4HDF5StoreParameter*>(StorePar);
           SetHDF5Store(hstor->GetName(), hstor->GetHDF5Flags());
                     break;
                  }
#endif


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
   if (!fStepStatus || !fStepStatus->GetSourcePar()) return kind_MbsFile;

   switch(fStepStatus->GetSourcePar()->GetID()) {
      case GO4EV_FILE: return kind_RootFile;
      case GO4EV_MBS_FILE: return kind_MbsFile;
      case GO4EV_MBS_STREAM: return kind_MbsStream;
      case GO4EV_MBS_TRANSPORT: return kind_MbsTransport;
      case GO4EV_MBS_EVENTSERVER: return kind_MbsEvent;
      case GO4EV_MBS_REVSERV: return kind_MbsREvent;
      case GO4EV_MBS_RANDOM: return kind_MbsRandom;
      case GO4EV_USER: return kind_UserSource;
      case GO4EV_HDF5: return kind_HDF5;
   } // SourcePar->GetID()

   return kind_MbsFile;
}

void TGo4ConfigStep::ChangeSourceParameter(int kind)
{
   if (!fStepStatus) return;

   TGo4EventSourceParameter* srcpar = fStepStatus->TakeSourcePar();
   bool delsrcpar(true);
   TGo4MbsSourceParameter* mbspar = dynamic_cast<TGo4MbsSourceParameter*> (srcpar);

   if (fLastSrcKind >= 0) {
      if (fPars[fLastSrcKind]) delete fPars[fLastSrcKind];
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
      case GO4EV_HDF5:
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

   TGo4EventSourceParameter* newpar = nullptr;

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
#ifdef __GO4HDF5__
      case kind_HDF5:
        newpar = new TGo4HDF5SourceParameter(filename);
        break;
#endif
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

   SpinBoxPortNumber->setVisible(false);
   TextLabelPortNumber->setVisible(false);
   SpinBoxRetryNumber->setVisible(false);
   TextLabelRetryNumber->setVisible(false);
   LineEditArgs->setVisible(false);
   TextLabelArgs->setVisible(false);
   LineEditTagfile->setVisible(false);
   TextLabelTagfile->setVisible(false);

   SpinBoxStartEvent->setVisible(false);
   SpinBoxStopEvent->setVisible(false);
   SpinBoxInterEvent->setVisible(false);
   SpinBoxTimeout->setVisible(false);
   TextLabelTimeout->setVisible(false);
   FileNameBtn->setEnabled(false);
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

//      if ((mbspar->GetPort() != 0) || (mbspar->GetRetryCnt() > 0) ||
//          (mbspar->GetStartEvent() != 0) || (mbspar->GetStopEvent() != 0) || (mbspar->GetEventInterval()>1)) fExtra = true;
   }

   TGo4MbsFileParameter* mbsfilpar = dynamic_cast<TGo4MbsFileParameter*>(srcpar);
   TGo4UserSourceParameter* userpar = dynamic_cast<TGo4UserSourceParameter*>(srcpar);

   switch (kind) {
      case kind_RootFile:            // root file with one tree
         FileNameBtn->setEnabled(true);
         break;
      case kind_MbsFile: {            // mbs listmode file (input only)
         // in this special case spin boxes will be enabled/disabled when file name is changed
         FileNameBtn->setEnabled(true);

         QString tagfile = mbsfilpar->GetTagName();

         if(!tagfile.contains(TGo4MbsFile__fgcNOTAGFILE)) {
            LineEditTagfile->setText(tagfile);
//            fExtra = true;
         } else {
            LineEditTagfile->setText("");
         }

         LineEditTagfile->setVisible(fExtra);
         TextLabelTagfile->setVisible(fExtra);

         SpinBoxStartEvent->setVisible(fExtra);
         SpinBoxStopEvent->setVisible(fExtra);
         SpinBoxInterEvent->setVisible(fExtra);
         break;
      }
      case kind_MbsStream:       // mbs stream server (input only)
      case kind_MbsTransport:    // mbs transport server (input only)
      case kind_MbsEvent:        // mbs event server  (input only)
      case kind_MbsREvent:       // rev serv
         TextLabelTimeout->setVisible(fExtra);
         SpinBoxTimeout->setVisible(fExtra);
         TextLabelPortNumber->setVisible(fExtra);
         SpinBoxPortNumber->setVisible(fExtra);
         TextLabelRetryNumber->setVisible(fExtra);
         SpinBoxRetryNumber->setVisible(fExtra);
         MbsMonitorBtn->setEnabled(kind != kind_MbsREvent);
         SpinBoxStartEvent->setVisible(fExtra);
         SpinBoxStopEvent->setVisible(fExtra);
         SpinBoxInterEvent->setVisible(fExtra);
         break;

      case kind_MbsRandom:       //    mbs random
         break;

      case kind_UserSource:      // user source
         FileNameBtn->setEnabled(true);
         SpinBoxPortNumber->setValue(userpar->GetPort());
         LineEditArgs->setText(userpar->GetExpression());
         SpinBoxPortNumber->setVisible(fExtra);
         TextLabelPortNumber->setVisible(fExtra);
         LineEditArgs->setVisible(fExtra);
         TextLabelArgs->setVisible(fExtra);
         TextLabelTimeout->setVisible(fExtra);
         SpinBoxTimeout->setVisible(fExtra);
         break;

      case kind_HDF5:            // hdf5 file
        FileNameBtn->setEnabled(true);
        break;
   }

   fBlocked--;

   adjustSize();
   parentWidget()->adjustSize();
   parentWidget()->parentWidget()->adjustSize();

   fxPanel->adjustSize();
   fxPanel->parentWidget()->adjustSize();
}

void TGo4ConfigStep::StoreComboHighlighted(int k)
{
   if(k == 0) {
      StoreNameEdit->setDisabled(false);
      TGo4FileStoreParameter newpar0(StoreNameEdit->text().toLatin1().constData());
      fStepStatus->SetStorePar(&newpar0);
      SplitLevel->setDisabled(false);
      BufferSize->setDisabled(false);
      CompLevel->setDisabled(false);
      StoreOverwriteMode->setDisabled(false);
      FileNameOutput->setDisabled(false);
      TreeAutosave->setDisabled(false);
   } else if(k==1) {
      StoreNameEdit->setDisabled(true);
      TGo4BackStoreParameter newpar1(GetBackStoreName().toLatin1().constData());
      fStepStatus->SetStorePar(&newpar1);
      SplitLevel->setDisabled(false);
      BufferSize->setDisabled(false);
      CompLevel->setDisabled(true);
      StoreOverwriteMode->setDisabled(true);
      FileNameOutput->setDisabled(true);
      TreeAutosave->setDisabled(true);
    } else if(k==2) {
       StoreNameEdit->setDisabled(false);
       TGo4UserStoreParameter newpar2(StoreNameEdit->text().toLatin1().constData());
       fStepStatus->SetStorePar(&newpar2);
       SplitLevel->setDisabled(true);
       BufferSize->setDisabled(true);
       CompLevel->setDisabled(true);
       StoreOverwriteMode->setDisabled(true);
       FileNameOutput->setDisabled(false);
       TreeAutosave->setDisabled(true);
     } else if(k==3) {
#ifdef __GO4HDF5__
      StoreNameEdit->setDisabled(false);
      TGo4HDF5StoreParameter newpar3(StoreNameEdit->text().toLatin1().constData(), GO4_H5F_ACC_TRUNC);
      fStepStatus->SetStorePar(&newpar3);
      StoreOverwriteMode->setDisabled(false);
      FileNameOutput->setDisabled(false);
      CompLevel->setDisabled(true);
      TreeAutosave->setDisabled(true);
      SplitLevel->setDisabled(true);
      BufferSize->setDisabled(true);
#endif
   }
}

void TGo4ConfigStep::OutputFileDialog()
{
   QString filters;
   if(fStepStatus) {
        TGo4EventStoreParameter* storepar = fStepStatus->GetStorePar();
        if(storepar->InheritsFrom(TGo4FileStoreParameter::Class()))
           filters = "Go4FileSource  (*.root)";
        else if (storepar->InheritsFrom(TGo4UserStoreParameter::Class()))
           filters = "Go4UserStore  (*.root)";
#ifdef __GO4HDF5__
        else if (storepar->InheritsFrom(TGo4HDF5StoreParameter::Class())) {
           filters="Go4HDF5 (*.h5)";
        }
#endif
        else
           std::cout <<"Unknown storepar " <<storepar->ClassName() << std::endl;
    }

   QFileDialog fd( this, "Select file name for step output",
         fxPanel->GetStorePath(), filters);
   fd.setFileMode( QFileDialog::AnyFile);
   if ( fd.exec() != QDialog::Accepted ) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString fileName = flst[0];
   std::cout << "output file is "<<fileName.toLatin1().constData() << std::endl;
   fxPanel->SetStorePath(fd.directory().path());
   if(!fileName.endsWith(".h5")) fileName.append(".h5");
   std::cout << "output file is now "<<fileName.toLatin1().constData() << std::endl;
   StoreNameEdit->setText(fileName);
}

void TGo4ConfigStep::StoreBufferSize(int t)
{
   if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())){
       TGo4FileStoreParameter *StorePar = (TGo4FileStoreParameter *)fStepStatus->GetStorePar();
       StorePar->SetBufsize(1000*t);
   } else
   if (fStepStatus->GetStorePar()->InheritsFrom(TGo4BackStoreParameter::Class())){
       TGo4BackStoreParameter * StorePar = (TGo4BackStoreParameter *)fStepStatus->GetStorePar();
       StorePar->SetBufsize(1000*t);
   }
}

void TGo4ConfigStep::StoreSplitLevel(int t)
{
   if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())) {
      TGo4FileStoreParameter *StorePar = (TGo4FileStoreParameter *)fStepStatus->GetStorePar();
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
#ifdef __GO4HDF5__
   else if(fStepStatus->GetStorePar()->InheritsFrom(TGo4HDF5StoreParameter::Class())){
     TGo4HDF5StoreParameter *StorePar=dynamic_cast <TGo4HDF5StoreParameter *> (fStepStatus->GetStorePar());
     StorePar->SetHDF5Flags(overwrite ? GO4_H5F_ACC_TRUNC : GO4_H5F_ACC_RDWR);
   }
#endif
}


void TGo4ConfigStep::StoreTreeAutoSave( int t )
{
  if(fStepStatus->GetStorePar()->InheritsFrom(TGo4FileStoreParameter::Class())) {
        TGo4FileStoreParameter *StorePar=(TGo4FileStoreParameter *)fStepStatus->GetStorePar();
        StorePar->SetAutosaveSize(t);
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

   if (!filpar) return;

   if(tag.isEmpty())
      filpar->SetTagName(TGo4MbsFile__fgcNOTAGFILE);
   else
      filpar->SetTagName(tag.trimmed().toLatin1().constData());
}

void TGo4ConfigStep::ChangeStartEvent( int num )
{
   if (fBlocked) return;

   // only for mbs sources
   TGo4MbsSourceParameter* par =
      dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
   if (par) par->SetStartEvent(num);
}

void TGo4ConfigStep::ChangeStopEvent( int num )
{
   if (fBlocked) return;

  // only for mbs sources
  TGo4MbsSourceParameter* par =
     dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
  if (par) par->SetStopEvent(num);
}

void TGo4ConfigStep::ChangeEventInterval( int num )
{
   if (fBlocked) return;

   // only for mbs sources
   TGo4MbsSourceParameter* par =
      dynamic_cast<TGo4MbsSourceParameter*>(fStepStatus->GetSourcePar());
   if (par) par->SetEventInterval(num);
}

void TGo4ConfigStep::ChangeSourceTimeout( int tim )
{
   if (fBlocked) return;

   TGo4EventSourceParameter* sourcepar = fStepStatus->GetSourcePar();
   if (sourcepar)
      sourcepar->SetTimeout(tim);
}

void TGo4ConfigStep::InputFileDialog()
{
   QString filters;
   bool mbsfilemode = false;
   if(fStepStatus) {
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
#ifdef __GO4HDF5__
        else if (sourcepar->InheritsFrom(TGo4HDF5SourceParameter::Class())) {
           filters="Go4HDF5 (*.h5)";
        }
#endif
       else
          std::cout <<"Unknown sourcepar " <<sourcepar->ClassName() << std::endl;
   }

    //std::cout <<"FFFFFFFF chose filter-"<< filters.toLatin1().constData() <<"-" << std::endl;
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

       LineEditTagfile->setVisible(!islml);
       TextLabelTagfile->setVisible(!islml);
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

void TGo4ConfigStep::SetHDF5Source(QString name)
{
#ifdef __GO4HDF5__
   EventSourceCombo->setCurrentIndex(kind_HDF5);
   LineEditArgs->setText(name);
#endif
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
         if (par) {
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
//   if (TagFile.isEmpty()) TagFile = TGo4MbsFile__fgcNOTAGFILE;
}

void TGo4ConfigStep::GetUserSource(int& port, QString& expr)
{
   port = SpinBoxPortNumber->value();
   expr = LineEditArgs->text();
}

void TGo4ConfigStep::SetFileStore(QString name, bool overwrite, int bufsize, int splitlevel, int compression, int autosave)
{
   StoreNameEdit->setEnabled(true);
   StoreNameEdit->setText(name);
   OutputCombo->setCurrentIndex(0);
   StoreOverwriteMode->setChecked(overwrite);
   StoreOverwriteMode->setEnabled(true);
   BufferSize->setEnabled(true);
   SplitLevel->setEnabled(true);
   BufferSize->setValue(bufsize/1000);
   SplitLevel->setValue(splitlevel);
   CompLevel->setValue(compression);
   TreeAutosave->setValue(autosave);
   CompLevel->setEnabled(true);
   FileNameOutput->setEnabled(true);
   StoreComboHighlighted(0);
}

void TGo4ConfigStep::SetBackStore(QString name, int bufsize, int splitlevel)
{
   StoreNameEdit->setEnabled(false);
   StoreNameEdit->setText(GetBackStoreName());
   OutputCombo->setCurrentIndex(1);
   BufferSize->setEnabled(true);
   SplitLevel->setEnabled(true);
   BufferSize->setValue(bufsize/1000);
   SplitLevel->setValue(splitlevel);
   StoreOverwriteMode->setEnabled(false);
   CompLevel->setEnabled(false);
   FileNameOutput->setEnabled(false);
   StoreComboHighlighted(1);
}

void TGo4ConfigStep::SetUserStore(QString name)
{
   StoreNameEdit->setEnabled(true);
   StoreNameEdit->setText(name);
   OutputCombo->setCurrentIndex(2);

   BufferSize->setEnabled(false);
   SplitLevel->setEnabled(false);
   StoreOverwriteMode->setEnabled(false);
   CompLevel->setEnabled(false);
   FileNameOutput->setEnabled(true);

   StoreComboHighlighted(2);
}

void TGo4ConfigStep::SetHDF5Store(QString name, int flags)
{
#ifdef __GO4HDF5__
   StoreNameEdit->setEnabled(false);
   StoreNameEdit->setText(name);
   OutputCombo->setCurrentIndex(3);
   StoreOverwriteMode->setEnabled(true);
   StoreOverwriteMode->setChecked(flags==GO4_H5F_ACC_TRUNC ? true : false);
// JAM2019: here evaluate the file access flags
//   Valid values of flags include://
//       H5F_ACC_TRUNC - Truncate file, if it already exists, erasing all data previously stored in the file.
//       H5F_ACC_EXCL - Fail if file already exists. H5F_ACC_TRUNC and H5F_ACC_EXCL are mutually exclusive
//       H5F_ACC_RDONLY - Open file as read-only, if it already exists, and fail, otherwise
//       H5F_ACC_RDWR - Open file for read/write, if it already exists, and fail, otherwise
   FileNameOutput->setEnabled(true);
   StoreComboHighlighted(3);
#endif
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

void TGo4ConfigStep::GetFileStore(bool& overwrite, int& bufsize, int& splitlevel, int& compression,  int& autosaveperiod)
{
   overwrite = StoreOverwriteMode->isChecked();
   bufsize = BufferSize->value() * 1000;
   splitlevel = SplitLevel->value();
   compression = CompLevel->value();
   autosaveperiod = TreeAutosave->value();
}

void TGo4ConfigStep::GetBackStore(int& bufsize, int& splitlevel)
{
   bufsize = BufferSize->value() * 1000;
   splitlevel = SplitLevel->value();
}

void TGo4ConfigStep::GetHDF5Store( int & flags)
{
#ifdef __GO4HDF5__
  StoreOverwriteMode->isChecked() ? flags=GO4_H5F_ACC_TRUNC : flags=0;
#endif
}

void TGo4ConfigStep::MbsMonitorBtn_clicked()
{
   if (fxPanel)
     fxPanel->DisplayMbsMonitor(SourceNameEdit->text());
}

void TGo4ConfigStep::ExtraBtn_clicked()
{
   fExtra = !fExtra;
   ExtraBtn->setText(fExtra ? "-" : "+");

   fBlocked++;

   SourceComboHighlighted(EventSourceCombo->currentIndex());

   fBlocked--;
}
