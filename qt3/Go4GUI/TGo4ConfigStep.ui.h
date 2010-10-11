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

const char* TGo4MbsFile__fgcNOTAGFILE = "GO4-NOLMDTAG";
const char* TGo4MbsFile__fgcLMDSUF = ".lmd";
const char* TGo4MbsFile__fgcFILELISTSUF = ".lml";

void TGo4ConfigStep::init()
{
   fxPanel = 0;
   fStepStatus = 0;
}

void TGo4ConfigStep::destroy()
{
}

void TGo4ConfigStep::InputArguments(const QString& Arg)
{
    TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
    int ParId=SourcePar->GetID();
    if (ParId==GO4EV_USER){                    // user defined source class
        TGo4UserSourceParameter* usrpar=dynamic_cast<TGo4UserSourceParameter*>(SourcePar);
        if (usrpar!=0)
           usrpar->SetExpression(Arg.stripWhiteSpace().latin1());
    }
}

void TGo4ConfigStep::InputStateChanged(int )
{
   bool stepon = EnableStepBox->isChecked();
   bool on = EnableSourceBox->isChecked();

   fStepStatus->SetSourceEnabled(on);
   SourceBox->setEnabled(stepon && on);
}

void TGo4ConfigStep::InputPort(const QString& fxPort)
{
    TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
    int ParId=SourcePar->GetID();
    if (ParId==GO4EV_MBS_REVSERV){              // remote event server (input only)
           TGo4RevServParameter* revpar=dynamic_cast<TGo4RevServParameter*>(SourcePar);
        if (revpar!=0){
                revpar->SetPort(fxPort.toInt());
        }
    }else if (ParId==GO4EV_USER){                    // user defined source class
        TGo4UserSourceParameter* usrpar=dynamic_cast<TGo4UserSourceParameter*>(SourcePar);
        if (usrpar!=0){
                usrpar->SetPort(fxPort.toInt());

        }
    }
}

void TGo4ConfigStep::InputSourceText(const QString& Name)
{
    fStepStatus->GetSourcePar()->SetName(Name.stripWhiteSpace().latin1());
}

void TGo4ConfigStep::OutputStateChanged(int )
{
   bool stepon = EnableStepBox->isChecked();
   bool on = EnableStoreBox->isChecked();

   fStepStatus->SetStoreEnabled(on);
   StoreBox->setEnabled(stepon && on);
   StoreBox->setShown(on);

   parentWidget()->adjustSize();
   parentWidget()->parentWidget()->adjustSize();
   parentWidget()->parentWidget()->parentWidget()->adjustSize();
}

void TGo4ConfigStep::OutputNameText(const QString& Name)
{
    fStepStatus->GetStorePar()->SetName(Name.stripWhiteSpace().latin1());
}

void TGo4ConfigStep::StepStateChanged(int )
{
   bool on = EnableStepBox->isChecked();

   fStepStatus->SetProcessEnabled(on);
   EnableSourceBox->setEnabled(on);
   EnableStoreBox->setEnabled(on);

   InputStateChanged(0);
   OutputStateChanged(0);
}

void TGo4ConfigStep::OutArguments(const QString&)
{

}


void TGo4ConfigStep::SetStepStatus(TGo4AnalysisConfiguration* panel, TGo4AnalysisStepStatus* StepStatus)
{
    if (StepStatus==0) return;

    fxPanel = panel;
    fStepStatus = StepStatus;

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
    LineEditPortNumber->setEnabled(false);
    LineEditTagfile->setEnabled(false);
    SpinBoxStartEvent->setEnabled(false);
    SpinBoxStopEvent->setEnabled(false);
    SpinBoxInterEvent->setEnabled(false);
    SpinBoxTimeout->setEnabled(false);
    LineEditArgumentsIn->setEnabled(false);
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
   
   
    if(k==0) {            // root file with one tree
       TGo4FileSourceParameter* newpar1 = new TGo4FileSourceParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar1);
       FileNameInput->setEnabled(true);
       if(rootfilpar==0) SourceNameEdit->clear();
       delete newpar1;
    } else
    if (k==1) {    // mbs listmode file (input only)
       TGo4MbsFileParameter* newpar4 = new TGo4MbsFileParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar4);
       
       if(SourceNameEdit->text().contains(TGo4MbsFile__fgcFILELISTSUF)) // case of *.lml
        {
           // case of *.lml file: disable start/stop event fields
           LineEditTagfile->setEnabled(false);
           SpinBoxStartEvent->setEnabled(false);
           SpinBoxStopEvent->setEnabled(false);
           SpinBoxInterEvent->setEnabled(false);
        }
       else
        {
           // case of *.lmd file
           LineEditTagfile->setEnabled(true);
           SpinBoxStartEvent->setEnabled(true);
           SpinBoxStopEvent->setEnabled(true);
           SpinBoxInterEvent->setEnabled(true);
        }
       FileNameInput->setEnabled(true);
       if(mbsfilpar==0) SourceNameEdit->clear();
       delete newpar4;
     } else
     if (k==2) {  // mbs stream server (input only)
       TGo4MbsStreamParameter* newpar5 = new TGo4MbsStreamParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar5);
       SpinBoxTimeout->setEnabled(true);
       MbsMonitorBtn->setEnabled(true);
       SpinBoxStartEvent->setEnabled(true);
       SpinBoxStopEvent->setEnabled(true);
       SpinBoxInterEvent->setEnabled(true);
       if(wasfilesource || randsrc!=0 ) SourceNameEdit->clear();  
       delete newpar5;
     } else
     if (k==3) {       // mbs transport server (input only)
       TGo4MbsTransportParameter* newpar6 = new TGo4MbsTransportParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar6);
       SpinBoxTimeout->setEnabled(true);
       MbsMonitorBtn->setEnabled(true);
       SpinBoxStartEvent->setEnabled(true);
       SpinBoxStopEvent->setEnabled(true);
       SpinBoxInterEvent->setEnabled(true);
       if(wasfilesource || randsrc!=0) SourceNameEdit->clear();
       delete newpar6;
     }else
     if (k==4) {     // mbs event server  (input only)
       TGo4MbsEventServerParameter* newpar7 = new TGo4MbsEventServerParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar7);
       SpinBoxTimeout->setEnabled(true);
       MbsMonitorBtn->setEnabled(true);
       SpinBoxStartEvent->setEnabled(true);
       SpinBoxStopEvent->setEnabled(true);
       SpinBoxInterEvent->setEnabled(true);
       if(wasfilesource || randsrc!=0) SourceNameEdit->clear();       
       delete newpar7;
     } else
     if (k==5) {     //    rev serv
       TGo4RevServParameter* newpar8 = new TGo4RevServParameter(SourceNameEdit->text().latin1());
       newpar8->SetPort(LineEditPortNumber->text().toInt());
       fStepStatus->SetSourcePar(newpar8);
       LineEditPortNumber->setEnabled(true);
       SpinBoxStartEvent->setEnabled(true);
       SpinBoxStopEvent->setEnabled(true);
       SpinBoxInterEvent->setEnabled(true);
       if(wasfilesource || randsrc!=0) SourceNameEdit->clear();
       delete newpar8;
     } else
     if (k==6) {     //    mbs random
       TGo4MbsRandomParameter* newpar8 = new TGo4MbsRandomParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar8);
       SourceNameEdit->setText("RandomEvents");
       delete newpar8;
     } else
     if (k==7) {     // user source
       TGo4UserSourceParameter *newpar9 = new TGo4UserSourceParameter(SourceNameEdit->text().latin1());
       newpar9->SetPort(LineEditPortNumber->text().toInt());
       newpar9->SetExpression(LineEditArgumentsIn->text().stripWhiteSpace().latin1());
       fStepStatus->SetSourcePar(newpar9);
       LineEditPortNumber->setEnabled(true);
       LineEditArgumentsIn->setEnabled(true);
       SpinBoxTimeout->setEnabled(true);
       FileNameInput->setEnabled(true);
       if(usersrc==0) SourceNameEdit->clear();       
       delete newpar9;
     }
}

void TGo4ConfigStep::StoreComboHighlighted(int k)
{
   if(k==0) {
      StoreNameEdit->setDisabled(FALSE);
      TGo4FileStoreParameter* newpar1 = new TGo4FileStoreParameter(StoreNameEdit->text().latin1());
      fStepStatus->SetStorePar(newpar1);
      delete newpar1;
      CompLevel->setDisabled(FALSE);
      StoreOverwriteMode->setDisabled(FALSE);
      FileNameOutput->setDisabled(FALSE);
   } else
   if(k==1) {
      StoreNameEdit->setDisabled(TRUE);
      TGo4BackStoreParameter *newpar3 = new TGo4BackStoreParameter(GetBackStoreName().latin1());
      fStepStatus->SetStorePar(newpar3);
      delete newpar3;
      CompLevel->setDisabled(TRUE);
      StoreOverwriteMode->setDisabled(TRUE);
      FileNameOutput->setDisabled(TRUE);
    }
}


void TGo4ConfigStep::OutputFileDialog()
{
    QFileDialog fd( this, "file name", TRUE );
    fd.setMode( QFileDialog::AnyFile);
    fd.setCaption( "Select file name for step output");
    fd.setDir(fxPanel->GetStorePath());
    fd.setFilter( "Go4FileStore  (*.root)" );
    if ( fd.exec() != QDialog::Accepted ) return;

    QString fileName = fd.selectedFile();
    fxPanel->SetStorePath(fd.dirPath());
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
   TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(SourcePar);
   if (filpar==0) return;

   if(tag.isEmpty())
      filpar->SetTagName(TGo4MbsFile__fgcNOTAGFILE);
   else
      filpar->SetTagName(tag.stripWhiteSpace().latin1());
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
    QFileDialog fd( this, "file name", TRUE );
    fd.setMode( QFileDialog::ExistingFile);
    fd.setCaption( "Select file name for step input");
    fd.setDir(fxPanel->GetSourcePath());
    bool mbsfilemode=false;
    if(fStepStatus!=0) {
        TGo4EventSourceParameter* sourcepar = fStepStatus->GetSourcePar();
        if(sourcepar->InheritsFrom(TGo4FileSourceParameter::Class()))
           fd.setFilter( "Go4FileSource  (*.root)" );
        else if (sourcepar->InheritsFrom(TGo4MbsFileParameter::Class())) {
            mbsfilemode=true;
            QString filters="Go4MbsFile  (*";
            filters+=TGo4MbsFile__fgcLMDSUF;
            filters+=" *";
            filters+=QString(TGo4MbsFile__fgcLMDSUF).upper();
            filters+=");;Go4 list mode list (*";
            filters+=TGo4MbsFile__fgcFILELISTSUF;
            filters+=")";
            fd.setFilters(filters);
        }
        else if (sourcepar->InheritsFrom(TGo4UserSourceParameter::Class())){
           fd.setFilter( "all files  (*.*)" );
        }
        else
           cout <<"Unknown sourcepar " <<sourcepar->ClassName() << endl;
    }

    if ( fd.exec() != QDialog::Accepted ) return;

    QString fileName = fd.selectedFile();
    fxPanel->SetSourcePath(fd.dirPath());
    SourceNameEdit->setText(fileName);
    if(mbsfilemode) {
       if(fd.selectedFilter().contains(TGo4MbsFile__fgcFILELISTSUF)) {
           LineEditTagfile->setEnabled(false);
           SpinBoxStartEvent->setEnabled(false);
           SpinBoxStopEvent->setEnabled(false);
           SpinBoxInterEvent->setEnabled(false);
        } else {
           LineEditTagfile->setEnabled(true);
           SpinBoxStartEvent->setEnabled(true);
           SpinBoxStopEvent->setEnabled(true);
           SpinBoxInterEvent->setEnabled(true);
        }
    }
}

QString TGo4ConfigStep::GetStepName()
{
   return fStepStatus->GetName();
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

void TGo4ConfigStep::ResetSourceWidgets(const QString& name, int timeout, int start, int stop, int interval)
{
    SourceNameEdit->setText(name);
    SpinBoxTimeout->setValue(timeout);
    SpinBoxStartEvent->setValue(start);
    SpinBoxStopEvent->setValue(stop);
    SpinBoxInterEvent->setValue(interval);

    LineEditPortNumber->setEnabled(false);
    LineEditArgumentsIn->setEnabled(false);
    LineEditTagfile->setEnabled(false);
    SpinBoxStartEvent->setEnabled(false);
    SpinBoxStopEvent->setEnabled(false);
    SpinBoxInterEvent->setEnabled(false);
    SpinBoxTimeout->setEnabled(false);
    FileNameInput->setEnabled(false);
}

void TGo4ConfigStep::SetFileSource()
{
   InputKindCombo->setCurrentItem(0);
   FileNameInput->setEnabled(true);
   SourceComboHighlighted(0);
}

void TGo4ConfigStep::SetMbsFileSource(QString TagFile)
{
   InputKindCombo->setCurrentItem(1);
   LineEditTagfile->setEnabled(true);
   FileNameInput->setEnabled(true);
   if(!strstr(TagFile.latin1(),TGo4MbsFile__fgcNOTAGFILE))
      LineEditTagfile->setText(TagFile);
   else
      LineEditTagfile->setText("");
   SourceComboHighlighted(1);
}

void TGo4ConfigStep::SetMbsStreamSource()
{
   InputKindCombo->setCurrentItem(2);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(2);
}

void TGo4ConfigStep::SetMbsTransportSource()
{
   InputKindCombo->setCurrentItem(3);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(3);
}

void TGo4ConfigStep::SetMbsEventServerSource()
{
   InputKindCombo->setCurrentItem(4);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(4);
}

void TGo4ConfigStep::SetMbsRevServSource(int port)
{
   InputKindCombo->setCurrentItem(5);
   QString PortNo;
   PortNo.setNum(port);
   LineEditPortNumber->setText(PortNo);
   LineEditPortNumber->setEnabled(true);
   LineEditArgumentsIn->setEnabled(true);
   SourceComboHighlighted(5);
}

void TGo4ConfigStep::SetRandomSource()
{
   InputKindCombo->setCurrentItem(6);
   SourceComboHighlighted(6);
}

void TGo4ConfigStep::SetUserSource(int port, QString expr)
{
   InputKindCombo->setCurrentItem(7);
   QString PortNo;
   PortNo.setNum(port);
   LineEditPortNumber->setText(PortNo);
   LineEditArgumentsIn->setText(expr);
   LineEditPortNumber->setEnabled(true);
   LineEditArgumentsIn->setEnabled(true);
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
   return InputKindCombo->currentItem();
}

void TGo4ConfigStep::GetMbsFileSource(QString& TagFile)
{
   TagFile = LineEditTagfile->text();
//   if (TagFile.length()==0) TagFile = TGo4MbsFile__fgcNOTAGFILE;
}

void TGo4ConfigStep::GetMbsRevServSource(int& port)
{
   port = LineEditPortNumber->text().toInt();
}

void TGo4ConfigStep::GetUserSource(int& port, QString& expr)
{
   port = LineEditPortNumber->text().toInt();
   expr = LineEditArgumentsIn->text();
}

void TGo4ConfigStep::SetFileStore(QString name, bool overwrite, int bufsize, int splitlevel, int compression)
{
   StoreNameEdit->setEnabled(TRUE);
   StoreNameEdit->setText(name);
   OutputCombo->setCurrentItem(0);
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
   OutputCombo->setCurrentItem(1);
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
   int typ = OutputCombo->currentItem();
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
