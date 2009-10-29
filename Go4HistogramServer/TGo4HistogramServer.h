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

#ifndef TGO4HISTOGRAMSERVER_H
#define TGO4HISTOGRAMSERVER_H

#include "TGo4CommandReceiver.h"
#include "TString.h"

class TGo4AnalysisClient;
class TGo4Analysis;
class TGo4ThreadHandler;
class TGo4ObjConnectorTimer;
class TGo4Socket;

/**
 * Handles a histogram server (mbs histogram protocol) for the
 * histograms registered in the analysis.
 * Optionally, object server for root may be started using
 * Go4 protocoll style. This was disabled for V3 by default 09/2005
 * @author J. Adamczewski
 * @since 23-Sep-2002, reduced 14-Jan-2003
 */

class TGo4HistogramServer : public TGo4CommandReceiver {

   friend class TGo4HisConnectorRunnable;

   public:

      virtual ~TGo4HistogramServer();

      TGo4HistogramServer(TGo4AnalysisClient* owner,
                          const char* servername="localhost",
                          const char* password="go4",
                          Bool_t useobjectserver=kFALSE);

      TGo4HistogramServer();

      /**  Used by object connector runnable to serve client request */
      Int_t ServeObjectClient();

      /** returns the portnumber for object server connection */
      UInt_t GetObjPort();

      /** This method is used by the object connectortimer Notify to
        * connect or disconnect a transportchannel (TSocket) on demand;
        * the connection process itself is handled by the connector runnable */
      virtual Int_t TimerConnect();

      TGo4AnalysisClient* GetAnalysisClient() const { return fxAnalysisClient; }

      TGo4Analysis* GetAnalysis() const { return fxAnalysis; }

      /** histogram server connector thread name */
      static const char* fgcCONTHREADNAME; //!

      /** object server  connector thread name */
      static const char* fgcOBJTHREADNAME; //!

      /** This string is send as histogram name for termination connection */
      static const char* fgcSHUTDOWNNAME; //!

      /** command string for object server nameslist request */
      static const char* fgcCOMGETLIST; //!

      /** Period of connector timer, in ms */
      static const UInt_t fguTIMERPERIOD;         //!

      /** Maximum cycles to wait until transport is open */
      static const Int_t fgiOPENWAITCYCLES;        //!

      /** Time for each open wait cycle, in ms */
      static const UInt_t fguOPENWAITCYCLETIME;   //!

      /** Maximum cycles to wait until transport is closed */
      static const Int_t fgiCLOSEWAITCYCLES;      //!

      /** Time for each close wait cycle, in ms */
      static const UInt_t fguCLOSEWAITCYCLETIME;  //!

      /** Maximum cycles to wait until transport is connected */
      static const Int_t fgiCONNECTWAITCYCLES;    //!

      /** Time for each connect wait cycle, in ms */
      static const UInt_t fguCONNECTWAITCYCLETIME;  //!

   private:

      const char* ConnectorName() const { return fxConnectorName.Data(); }

      const char* ObjectThreadName() const { return fxObjectThreadName.Data(); }

      const char* ConnectHost() const { return fcConnectHost.Data(); }

      void SetConnect(TGo4Socket * trans, const char* host, UInt_t port);

      void SetDisConnect(TGo4Socket * trans);

      Int_t WaitForOpen();

      Int_t WaitForClose();

      Int_t WaitForConnection();

      /** Terminate connection to current object client, keep waiting  for the next one */
      void DisconnectObjectClient();

      /** Checks basename and passwd login of connected client  */
      Bool_t CheckLogin();

      /** Starts server socket waiting for the next client  */
      Int_t ConnectObjectClient();

      /** Ask for which object to send and give it to the client   */
      Bool_t HandleObjectRequest();

      /** Send object to the current object client.
        *  Protocol matches TGo4ObjectClient::ReceiveObject() */
      Bool_t SendObject(TObject* obj);

      TGo4AnalysisClient* fxAnalysisClient;//!
      TGo4Analysis* fxAnalysis;//!
      TGo4ThreadHandler* fxThreadHandler;//!

      /**  contains api server name  */
      TString fxServerName;    //!

      /**  contains api server password */
      TString fxServerPass;    //!

      /** Name of connector thread*/
      TString fxConnectorName; //!

      /** Name of object server thread*/
      TString fxObjectThreadName; //!

      /** port number for server socket*/
      Int_t fiServerPort;

      /** socket for object server **/
      TGo4Socket* fxTransport; //!

      /** port number for the object server connections */
      UInt_t fuObjectPort;

      /** link to the next TaskHandler transport instance that shall be connected
        * by the Object connector Timer with the client */
      TGo4Socket * fxConnectTransport; //!

      /** link to the next TaskHandler transport instance that shall be disconnected
        * by the connector  Timer with the client */
      TGo4Socket * fxDisConnectTransport; //!

      /** hostname for timer connect*/
      TString fcConnectHost; //!

      /** port number for timer connect */
      UInt_t fuConnectPort;

      /** True if fxConnectTransport shall be Open() by Timer */
      Bool_t fbConnectRequest;

      /** True if fxConnectTransport shall be Close() by Timer */
      Bool_t fbDisConnectRequest;

      /** True if fxConnectTransport waits in server Open() call  */
      Bool_t fbConnectIsOpen;

      /** True if fxConnectTransport has returned from Open(),
        * i.e. connection was established */
      Bool_t fbConnectIsDone;

      /** True if fxConnectTransport has returned from Close(),
        * i.e. connection was closed */
      Bool_t fbConnectIsClose;

      /** timer responsible for the connection/disconnection of clients;
       * independent of application control timer
       * @link aggregationByValue
       *   @supplierCardinality 1 */
      TGo4ObjConnectorTimer * fxConnectorTimer; //!

       /** Switches Go4 objectserver option on/off */
      Bool_t fbUseObjectServer;


  ClassDef(TGo4HistogramServer,1)
};

#endif //TGO4HISTOGRAMSERVER_H
