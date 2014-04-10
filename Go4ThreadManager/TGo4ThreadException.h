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

#ifndef TGo4ThreadException_h
#define TGo4ThreadException_h

#include "TGo4ControlException.h"
#include "TString.h"

class TGo4Runnable;
class TGo4ThreadHandler;

/**
 * Exception base class for all exceptions acting on a thread.
 * @author J. Adamczewski(at)gsi.de
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4ThreadException : public TGo4ControlException {
   public:
      TGo4ThreadException(const TGo4ThreadException &right);

      TGo4ThreadException (TGo4Runnable* runnable, const char* threadname = 0);

      virtual ~TGo4ThreadException();

      TGo4ThreadException & operator=(const TGo4ThreadException &right);

      /** This is a default handler function for the respective exception type. */
      virtual Int_t Handle () = 0;

      const char* GetThreadName ();

   protected:
      /** Name of the TGo4Thread  that shall be acted on */
      TString fxThreadName;

      /** Link to the runnable connected with the thread
        *   (exception throwing runnable might simply pass its 'this' pointer as argument) **/
      TGo4Runnable *fxRunnable; //!

      /** Link to threadhandler associated with the runnable **/
      TGo4ThreadHandler *fxThreadHandler; //!

   private:
      TGo4ThreadException();
};

#endif
