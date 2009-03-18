#include "TGo4ConfigStep.h"

#include <Q3FileDialog>
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
    : QWidget( parent, name, fl )
{ 
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI

   fxPanel = 0;
   fStepStatus = 0;
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

void TGo4ConfigStep::InputDisable(int )
{
    if(Disable_in_2->isChecked()) { //checked
        fStepStatus->SetSourceEnabled(kFALSE);
        SourceBox->setEnabled(false);
    } else {                        //unchecked
        fStepStatus->SetSourceEnabled(kTRUE);
        SourceBox->setEnabled(true);
    }
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

void TGo4ConfigStep::OutputDisable(int )
{
    if(Disable_out_2->isChecked()){ //checked
       fStepStatus->SetStoreEnabled(kFALSE);
       StoreBox->setEnabled(false);
       StoreBox->setHidden(true);
    }else{    //unchecked
       fStepStatus->SetStoreEnabled(kTRUE);
       StoreBox->setEnabled(true);
       StoreBox->setShown(true);
    }
   parentWidget()->parentWidget()->parentWidget()->adjustSize();
}

void TGo4ConfigStep::OutputNameText(const QString& Name)
{
    fStepStatus->GetStorePar()->SetName(Name.stripWhiteSpace().latin1());
}

void TGo4ConfigStep::StepDisable(int )
{
    if(Step_Disable_b->isChecked()){
       fStepStatus->SetProcessEnabled(kFALSE);
       SourceBox->setEnabled(false);
       StoreBox->setEnabled(false);
//       StoreBox->setHidden(true);
    }else{
       fStepStatus->SetProcessEnabled(kTRUE);
       InputDisable(0);
       OutputDisable(0);
   }

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

    ResetSourceWidgets(srcpar->GetName(), srcpar->GetTimeout());

    switch(srcpar->GetID()) {
       case GO4EV_FILE: {
          SetFileSource();
          break;
       }
       case GO4EV_MBS_FILE: {
          TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(srcpar);
          SetMbsFileSource(filpar ? filpar->GetTagName() : "",
                           filpar ? filpar->GetStartEvent() : 0,
                           filpar ? filpar->GetStopEvent() : 0,
                           filpar ? filpar->GetEventInterval() : 0);
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
    LineEditArgumentsIn_2->setEnabled(false);
    FileNameInput->setEnabled(false);
    MbsMonitorBtn->setEnabled(false);
    if(k==0) {            // root file with one tree
       TGo4FileSourceParameter* newpar1 = new TGo4FileSourceParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar1);
       FileNameInput->setEnabled(true);
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
       delete newpar4;
     } else
     if (k==2) {  // mbs stream server (input only)
       TGo4MbsStreamParameter* newpar5 = new TGo4MbsStreamParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar5);
       SpinBoxTimeout->setEnabled(true);
       MbsMonitorBtn->setEnabled(true);
       delete newpar5;
     } else
     if (k==3) {       // mbs transport server (input only)
       TGo4MbsTransportParameter* newpar6 = new TGo4MbsTransportParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar6);
       SpinBoxTimeout->setEnabled(true);
       MbsMonitorBtn->setEnabled(true);
       delete newpar6;
     }else
     if (k==4) {     // mbs event server  (input only)
       TGo4MbsEventServerParameter* newpar7 = new TGo4MbsEventServerParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar7);
       SpinBoxTimeout->setEnabled(true);
       MbsMonitorBtn->setEnabled(true);
       delete newpar7;
     } else
     if (k==5) {     //    rev serv
       TGo4RevServParameter* newpar8 = new TGo4RevServParameter(SourceNameEdit->text().latin1());
       newpar8->SetPort(LineEditPortNumber->text().toInt());
       fStepStatus->SetSourcePar(newpar8);
       LineEditPortNumber->setEnabled(true);
       delete newpar8;
     } else
     if (k==6) {     //    mbs random
       TGo4MbsRandomParameter* newpar8 = new TGo4MbsRandomParameter(SourceNameEdit->text().latin1());
       fStepStatus->SetSourcePar(newpar8);
       delete newpar8;
     } else
     if (k==7) {     // user source
       TGo4UserSourceParameter *newpar9 = new TGo4UserSourceParameter(SourceNameEdit->text().latin1());
       newpar9->SetPort(LineEditPortNumber->text().toInt());
       newpar9->SetExpression(LineEditArgumentsIn_2->text().stripWhiteSpace().latin1());
       fStepStatus->SetSourcePar(newpar9);
       LineEditPortNumber->setEnabled(true);
       LineEditArgumentsIn_2->setEnabled(true);
       SpinBoxTimeout->setEnabled(true);
       FileNameInput->setEnabled(true);
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
    Q3FileDialog fd( this, "file name", TRUE );
    fd.setMode( Q3FileDialog::AnyFile);
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
   if (ParId==GO4EV_MBS_FILE)
    {              // tagfile only for mbs lmd file
       TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(SourcePar);
    if (filpar!=0)
        {
            if(tag.isEmpty())
                {
                    //cout <<"Input empty tagname...." << endl;
                    filpar->SetTagName(TGo4MbsFile__fgcNOTAGFILE);
                }
            else
                {
                    filpar->SetTagName(tag.stripWhiteSpace().latin1());
                }

        }
    }
}

void TGo4ConfigStep::StoreStartEvent( int num )
{
   TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
   int ParId=SourcePar->GetID();
   if (ParId==GO4EV_MBS_FILE)
    {              //only for mbs lmd file
       TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(SourcePar);
    if (filpar!=0)
            filpar->SetStartEvent(num);
    }
}

void TGo4ConfigStep::StoreStopEvent( int num )
{
TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
int ParId=SourcePar->GetID();
if (ParId==GO4EV_MBS_FILE)
    {              //only for mbs lmd file
       TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(SourcePar);
    if (filpar!=0)
            filpar->SetStopEvent(num);
    }

}

void TGo4ConfigStep::StoreEventInterval( int num )
{
TGo4EventSourceParameter* SourcePar=fStepStatus->GetSourcePar();
int ParId=SourcePar->GetID();
if (ParId==GO4EV_MBS_FILE)
    {              //only for mbs lmd file
       TGo4MbsFileParameter* filpar=dynamic_cast<TGo4MbsFileParameter*>(SourcePar);
    if (filpar!=0)
            filpar->SetEventInterval(num);
    }
}

void TGo4ConfigStep::StoreTimeout( int tim )
{
   TGo4EventSourceParameter* sourcepar = fStepStatus->GetSourcePar();
   if (sourcepar!=0)
      sourcepar->SetTimeout(tim);
}

void TGo4ConfigStep::InputFileDialog()
{
    Q3FileDialog fd( this, "file name", TRUE );
    fd.setMode( Q3FileDialog::ExistingFile);
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
   Step_Disable_b->setChecked(!process);
   Disable_in_2->setChecked(!source);
   Disable_out_2->setChecked(!store);
}

void TGo4ConfigStep::GetStepControl(bool& process, bool& source, bool& store)
{
   process  = !Step_Disable_b->isChecked();
   source = !Disable_in_2->isChecked();
   store = !Disable_out_2->isChecked();
}

void TGo4ConfigStep::ResetSourceWidgets(const QString& name, int timeout)
{
    SourceNameEdit->setText(name);
    SpinBoxTimeout->setValue(timeout);

    LineEditPortNumber->setEnabled(false);
    LineEditArgumentsIn_2->setEnabled(false);
    LineEditTagfile->setEnabled(false);
    SpinBoxStartEvent->setEnabled(false);
    SpinBoxStopEvent->setEnabled(false);
    SpinBoxInterEvent->setEnabled(false);
    SpinBoxTimeout->setEnabled(false);
    FileNameInput->setEnabled(false);
}

void TGo4ConfigStep::SetFileSource()
{
   Input_Combo_1_2->setCurrentItem(0);
   FileNameInput->setEnabled(true);
   SourceComboHighlighted(0);
}

void TGo4ConfigStep::SetMbsFileSource(QString TagFile, int start, int stop, int interval)
{
   Input_Combo_1_2->setCurrentItem(1);
   LineEditTagfile->setEnabled(true);
   SpinBoxStartEvent->setEnabled(true);
   SpinBoxStopEvent->setEnabled(true);
   SpinBoxInterEvent->setEnabled(true);
   FileNameInput->setEnabled(true);
   if(!strstr(TagFile.latin1(),TGo4MbsFile__fgcNOTAGFILE))
      LineEditTagfile->setText(TagFile);
   else
      LineEditTagfile->setText("");
   SpinBoxStartEvent->setValue(start);
   SpinBoxStopEvent->setValue(stop);
   SpinBoxInterEvent->setValue(interval);
   SourceComboHighlighted(1);
}

void TGo4ConfigStep::SetMbsStreamSource()
{
   Input_Combo_1_2->setCurrentItem(2);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(2);
}

void TGo4ConfigStep::SetMbsTransportSource()
{
   Input_Combo_1_2->setCurrentItem(3);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(3);
}

void TGo4ConfigStep::SetMbsEventServerSource()
{
   Input_Combo_1_2->setCurrentItem(4);
   SpinBoxTimeout->setEnabled(true);
   SourceComboHighlighted(4);
}

void TGo4ConfigStep::SetMbsRevServSource(int port)
{
   Input_Combo_1_2->setCurrentItem(5);
   QString PortNo;
   PortNo.setNum(port);
   LineEditPortNumber->setText(PortNo);
   LineEditPortNumber->setEnabled(true);
   LineEditArgumentsIn_2->setEnabled(true);
   SourceComboHighlighted(5);
}

void TGo4ConfigStep::SetRandomSource()
{
   Input_Combo_1_2->setCurrentItem(6);
   SourceComboHighlighted(6);
}

void TGo4ConfigStep::SetUserSource(int port, QString expr)
{
   Input_Combo_1_2->setCurrentItem(7);
   QString PortNo;
   PortNo.setNum(port);
   LineEditPortNumber->setText(PortNo);
   LineEditArgumentsIn_2->setText(expr);
   LineEditPortNumber->setEnabled(true);
   LineEditArgumentsIn_2->setEnabled(true);
   SpinBoxTimeout->setEnabled(true);
   FileNameInput->setEnabled(true);
   SourceComboHighlighted(7);
}

int TGo4ConfigStep::GetSourceSetup(QString& name, int& timeout)
{
   name = SourceNameEdit->text();
   timeout = SpinBoxTimeout->value();
   return Input_Combo_1_2->currentItem();
}

void TGo4ConfigStep::GetMbsFileSource(QString& TagFile, int& start, int& stop, int& interval)
{
   TagFile = LineEditTagfile->text();
//   if (TagFile.length()==0) TagFile = TGo4MbsFile__fgcNOTAGFILE;
   start = SpinBoxStartEvent->value();
   stop = SpinBoxStopEvent->value();
   interval = SpinBoxInterEvent->value();
}

void TGo4ConfigStep::GetMbsRevServSource(int& port)
{
   port = LineEditPortNumber->text().toInt();
}

void TGo4ConfigStep::GetUserSource(int& port, QString& expr)
{
   port = LineEditPortNumber->text().toInt();
   expr = LineEditArgumentsIn_2->text();
}

void TGo4ConfigStep::SetFileStore(QString name, bool overwrite, int bufsize, int splitlevel, int compression)
{
   StoreNameEdit->setEnabled(TRUE);
   StoreNameEdit->setText(name);
   Output_Combo_1_2->setCurrentItem(0);
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
   Output_Combo_1_2->setCurrentItem(1);
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
   int typ = Output_Combo_1_2->currentItem();
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
