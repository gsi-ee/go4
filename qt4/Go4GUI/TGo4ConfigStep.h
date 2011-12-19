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

#ifndef TGO4CONFIGSTEP_H
#define TGO4CONFIGSTEP_H


#include <QtGui/QWidget>
#include "ui_TGo4ConfigStep.h"

class TGo4AnalysisConfiguration;
class TGo4AnalysisStepStatus;
class TGo4EventSourceParameter;

class TGo4ConfigStep : public QWidget, public Ui::TGo4ConfigStep
{
   Q_OBJECT

   public:
      TGo4ConfigStep( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
      virtual ~TGo4ConfigStep();
      virtual void SetStepStatus( TGo4AnalysisConfiguration * panel, TGo4AnalysisStepStatus * StepStatus, int number = -1 );
      virtual QString GetStepName();
      virtual QString GetTabTitle();
      virtual void SetStepControl( bool process, bool source, bool store );
      virtual void GetStepControl( bool & process, bool & source, bool & store );
      virtual void SetSourceWidgets(const QString & name, int timeout);
      virtual void SetMbsSourceWidgets(int start, int stop, int interval, int port = 0);
      virtual void SetFileSource();
      virtual void SetMbsFileSource(const QString& TagFile );
      virtual void SetMbsStreamSource();
      virtual void SetMbsTransportSource();
      virtual void SetMbsEventServerSource();
      virtual void SetMbsRevServSource( int port );
      virtual void SetMbsPort(int port);
      virtual void SetMbsRetryCnt(int retry);
      virtual void SetRandomSource();
      virtual void SetUserSource( int port, QString expr );
      virtual int GetSourceSetup( QString & name, int & timeout, int & start, int & stop, int & interval, int & port, int & nretry);
      virtual void GetMbsFileSource( QString & TagFile );
      virtual void GetUserSource( int & port, QString & expr );
      virtual void SetFileStore( QString name, bool overwrite, int bufsize, int splitlevel, int compression );
      virtual void SetBackStore( QString name, int bufsize, int splitlevel );
      virtual QString GetBackStoreName();
      virtual int GetStoreSetup( QString & name );
      virtual void GetFileStore( bool & overwrite, int & bufsize, int & splitlevel, int & compression );
      virtual void GetBackStore( int & bufsize, int & splitlevel );


   public slots:

      virtual void InputArguments( const QString & Arg );
      virtual void InputStateChanged( int );
      virtual void InputPortChanged( int );
      virtual void RetryCntChanged( int );
      virtual void InputSourceText( const QString & Name );
      virtual void OutputStateChanged( int );
      virtual void OutputNameText( const QString & Name );
      virtual void StepStateChanged( int );
      virtual void SourceComboHighlighted( int kind );
      virtual void StoreComboHighlighted( int k );
      virtual void OutputFileDialog();
      virtual void StoreBufferSize( int t );
      virtual void StoreSplitLevel( int t );
      virtual void StoreCompLevel( int t );
      virtual void StoreOverWrite( bool overwrite );
      virtual void InputTagfile( const QString & tag );
      virtual void ChangeStartEvent( int num );
      virtual void ChangeStopEvent( int num );
      virtual void ChangeEventInterval( int num );
      virtual void ChangeSourceTimeout( int tim );
      virtual void InputFileDialog();
      virtual void MbsMonitorBtn_clicked();
      virtual void ExtraBtn_clicked();

   protected:
      TGo4AnalysisConfiguration* fxPanel;
      TGo4AnalysisStepStatus* fStepStatus;
      int fStepNumber;
      int fLastSrcKind; // last selected kind of source parameter
      int fBlocked;    // indicates if all value-modified slots are blocked
      bool fExtra;     // show extra properties for event source

      enum { ParsSize = 8 };

      TGo4EventSourceParameter* fPars[ParsSize]; // array of parameters which are used to preserved once edited values

      void ChangeSourceParameter(int kind);
      int CurrentSourceKind();
};

#endif
