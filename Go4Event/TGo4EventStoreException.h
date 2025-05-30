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

#ifndef TGO4EVENTSTOREEXCEPTION_H
#define TGO4EVENTSTOREEXCEPTION_H

#include "TGo4RuntimeException.h"
#include "TString.h"

class TGo4EventStore;

/** @brief Event store exception
 * @ingroup go4_event
 */

class TGo4EventStoreException : public TGo4RuntimeException {

  public:

      TGo4EventStoreException() = delete;

      TGo4EventStoreException (TGo4EventStore *eventsource, const char *msg = nullptr);

      virtual ~TGo4EventStoreException();

      Int_t Handle() override;

      TGo4EventStoreException(const TGo4EventStoreException &right);

      TGo4EventStoreException &operator=(const TGo4EventStoreException &right);

      /** Status/ error message of the last event. */
      const char *GetErrMess() const { return fxErrMess.Data(); }

      /** Name of the throwing event source. */
      const char *GetStoreName() const { return fxStoreName.Data(); }

      /** Name of the throwing event source. */
      const char *GetStoreClass() const { return fxStoreClass.Data(); }

  protected:
      /** @supplierCardinality 1 */
      TGo4EventStore  *fxEventStore{nullptr};   //!

      /** Error message at exception throw time*/
      TString fxErrMess; //!

      /** Name of the throwing event source*/
      TString fxStoreName; //!

      /** Class of the throwing event source*/
      TString fxStoreClass; //!

};

#endif
