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

#ifndef TGO4EVENTSOURCEEXCEPTION_H
#define TGO4EVENTSOURCEEXCEPTION_H

#include "TGo4RuntimeException.h"
#include "TString.h"

class TGo4EventSource;

/** @brief Event source exception
 * @ingroup go4_event
 */
class TGo4EventSourceException : public TGo4RuntimeException {

  public:

      TGo4EventSourceException() = delete;

      TGo4EventSourceException (TGo4EventSource *eventsource);

      virtual ~TGo4EventSourceException();

      Int_t Handle() override;

      TGo4EventSourceException(const TGo4EventSourceException &right);

      TGo4EventSourceException &operator=(const TGo4EventSourceException &right);

      /** Status value of server/file open. */
      Int_t GetCreateStatus() const { return fiCreateStatus; }

      /** Status of the last event. */
      Int_t GetEventStatus() const { return fiEventStatus; }

      /** Status/ error message of the last event. */
      const char *GetErrMess() const { return fxErrMess.Data(); }

      /** Name of the throwing event source. */
      const char *GetSourceName() const { return fxSourceName.Data(); }

      /** Name of the throwing event source. */
      const char *GetSourceClass() const { return fxSourceClass.Data(); }

  protected:
      /** @supplierCardinality 1 */
      TGo4EventSource   *fxEventSource{nullptr};     //!

      /** Contains return value of eventsource open (e.g. f_evt_get_open) call. May be checked by the creating factory after return from source ctor. */
      Int_t fiCreateStatus{0};

      /** Contains return value of eventsource getevent (e.g. f_evt_get_event)  call. May be checked by the controlling event loop.*/
      Int_t fiEventStatus{0};

      /** Error message at exception throw time*/
      TString fxErrMess; //!

      /** Name of the throwing event source*/
      TString fxSourceName; //!

      /** Class of the throwing event source*/
      TString fxSourceClass; //!

};

#endif

