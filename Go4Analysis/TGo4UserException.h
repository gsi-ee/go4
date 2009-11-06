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

#ifndef TGO4USEREXCEPTION_H
#define TGO4USEREXCEPTION_H

#include "TGo4RuntimeException.h"

#include "TString.h"

/* will skip subsequent steps for this event silently and start next event */
#define GO4_SKIP_EVENT throw TGo4UserException(1,"");

/* will skip subsequent steps for this event and start next event,
 * will send printf style message to gui */
#define GO4_SKIP_EVENT_MESSAGE(...) throw TGo4UserException(1,__VA_ARGS__);

/* will stop analysis without message to gui */
#define GO4_STOP_ANALYSIS throw TGo4UserException(3,"");

/* will stop analysis with a printf style message to gui */
#define GO4_STOP_ANALYSIS_MESSAGE(...) throw TGo4UserException(3,__VA_ARGS__);



/**
 * Exception to be thrown by analysis user.
 * @author J. Adamczewski
 * @since Sep-2002
 */

class TGo4UserException : public TGo4RuntimeException {

   public:

      TGo4UserException (Int_t prio, const char* text,...);

      virtual ~TGo4UserException();

      Int_t Handle ();

      TGo4UserException(const TGo4UserException &right);

      TGo4UserException & operator = (const TGo4UserException & right);

      /** Message priority */
      Int_t GetPriority() const { return fiPriority; }

      /** Status message of last Process() call. */
      const char* GetMessage() const { return fxMessage.Data(); }

   protected:

      /** User defined description of the exception reason */
      TString fxMessage;

      /** Priority of message. 1=info, 2= warning, 3=error.*/
      Int_t fiPriority;

   private:
      TGo4UserException();
};

#endif // #define
