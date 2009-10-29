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

#ifndef TYYYEVENTSOURCE_H
#define TYYYEVENTSOURCE_H

#include "TGo4EventSource.h"
#include <fstream>

class TYYYRawEvent;
class TGo4UserSourceParameter;

class TYYYEventSource : public TGo4EventSource {
   public:

      TYYYEventSource();

      /** Create source specifying values directly */
      TYYYEventSource(const char* name, const char* args, Int_t port);

      /** Creat source from setup within usersource parameter object */
      TYYYEventSource(TGo4UserSourceParameter* par);

      virtual ~TYYYEventSource();


      /** Open the file or connection. */
      virtual Int_t Open();

      /** Close the file or connection. */
      virtual Int_t Close();


      /** This method checks if event class is suited for the source */
      virtual Bool_t CheckEventClass(TClass* cl);

      /** This methods actually fills the target event class which is passed as pointer.
        * Uses the latest event which is referenced
        * by fxEvent or fxBuffer. Does _not_ fetch a new event
        * from source, therefore one source event may be used
        * to fill several TYYYEvent classes. To get a new
        * event call NextEvent() before this method.*/
      virtual Bool_t BuildEvent(TGo4EventElement* dest);

      const char* GetArgs() const { return fxArgs.Data(); }

      void SetArgs(const char* arg) { fxArgs=arg; }

      Int_t GetPort() const { return fiPort; }

      void SetPort(Int_t val) { fiPort=val; }

   private:

      Bool_t fbIsOpen;

      /** Optional argument string */
      TString fxArgs;

      /** Optional port number  */
      Int_t fiPort;

      /** file that contains the data in ascii format. */
      std::ifstream* fxFile; //!

   ClassDef(TYYYEventSource, 1)
};

#endif //TGO4MBSSOURCE_H
