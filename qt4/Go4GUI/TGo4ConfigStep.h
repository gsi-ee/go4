#ifndef TGO4CONFIGSTEP_H
#define TGO4CONFIGSTEP_H


#include <QtGui/QWidget>
#include "ui_TGo4ConfigStep.h"

class TGo4AnalysisConfiguration;
class TGo4AnalysisStepStatus;



class TGo4ConfigStep : public QWidget, public Ui::TGo4ConfigStep
{
   Q_OBJECT

   public:
      TGo4ConfigStep( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
      virtual void SetStepStatus( TGo4AnalysisConfiguration * panel, TGo4AnalysisStepStatus * StepStatus, int number = -1 );
      virtual QString GetStepName();
      virtual QString GetTabTitle();
      virtual void SetStepControl( bool process, bool source, bool store );
      virtual void GetStepControl( bool & process, bool & source, bool & store );
      virtual void ResetSourceWidgets( const QString & name, int timeout, int start = 0, int stop = 0, int interval = 0);
      virtual void SetFileSource();
      virtual void SetMbsFileSource( QString TagFile );
      virtual void SetMbsStreamSource();
      virtual void SetMbsTransportSource();
      virtual void SetMbsEventServerSource();
      virtual void SetMbsRevServSource( int port );
      virtual void SetRandomSource();
      virtual void SetUserSource( int port, QString expr );
      virtual int GetSourceSetup( QString & name, int & timeout, int & start, int & stop, int & interval );
      virtual void GetMbsFileSource( QString & TagFile );
      virtual void GetMbsRevServSource( int & port );
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
      virtual void InputPort( const QString & fxPort );
      virtual void InputSourceText( const QString & Name );
      virtual void OutputStateChanged( int );
      virtual void OutputNameText( const QString & Name );
      virtual void StepStateChanged( int );
      virtual void OutArguments( const QString & );
      virtual void SourceComboHighlighted( int k );
      virtual void StoreComboHighlighted( int k );
      virtual void OutputFileDialog();
      virtual void StoreBufferSize( int t );
      virtual void StoreSplitLevel( int t );
      virtual void StoreCompLevel( int t );
      virtual void StoreOverWrite( bool overwrite );
      virtual void InputTagfile( const QString & tag );
      virtual void StoreStartEvent( int num );
      virtual void StoreStopEvent( int num );
      virtual void StoreEventInterval( int num );
      virtual void StoreTimeout( int tim );
      virtual void InputFileDialog();
      virtual void MbsMonitorBtn_clicked();

   protected:
      TGo4AnalysisConfiguration* fxPanel;
      TGo4AnalysisStepStatus* fStepStatus;
      int fStepNumber;

};

#endif
