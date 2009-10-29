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

#ifndef TGO4TASKHANDLERSTATUS_H
#define TGO4TASKHANDLERSTATUS_H

#include "TGo4Status.h"
#include "TString.h"

class TGo4TaskHandlerStatus : public TGo4Status {
    friend class TGo4TaskHandler;
   public:

      TGo4TaskHandlerStatus(); // default ctor for streamer

      virtual ~TGo4TaskHandlerStatus();

      const char* GetComName() const { return fxComName.Data(); }

      const char* GetStatName() const { return fxStatName.Data(); }

      const char* GetDatName() const { return fxDatName.Data(); }

      const char* GetHostName() const { return fxHostName.Data(); }

      UInt_t GetNegPort() const { return  fuNegPort; }

      Int_t GetComPort() const { return fiComPort; }

      Int_t GetStatPort() const { return fiStatPort; }

      Int_t GetDatPort() const { return fiDatPort; }

      Bool_t IsAborting() const { return fbIsAborting; }

      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

   private:
      TGo4TaskHandlerStatus(const char*);

      void SetFlags(Bool_t isaborting);

      void SetPorts(UInt_t neg, Int_t com, Int_t stat, Int_t dat);

      void SetNames(const char* com, const char* stat, const char* dat, const char* host);

      /** Remember name of command thread */
      TString fxComName;

      /** Remember name of status thread */
      TString  fxStatName;

      /** Remember name of data thread */
      TString  fxDatName;

      /** Remember name of remote task handler */
      TString  fxHostName;

      /** Actual port number of negotiation connection (raw transport) */
      UInt_t fuNegPort;

      /** Command connection port number */
      Int_t fiComPort;

      /** Status connection port number */
      Int_t fiStatPort;

      /** Data connection port number */
      Int_t fiDatPort;

      /** true if taskhandler is being aborted e.g. by remove exception */
      Bool_t fbIsAborting;

   ClassDef(TGo4TaskHandlerStatus,1)
};

#endif //TGO4TASKHANDLERSTATUS_H
