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

#ifndef TGO4MBSSOURCEPARAMETER_H
#define TGO4MBSSOURCEPARAMETER_H

#include "TGo4EventSourceParameter.h"

/** MBS source parameter
 * @ingroup go4_event
 */

class TGo4MbsSourceParameter : public TGo4EventSourceParameter {
  public:
     TGo4MbsSourceParameter();

     TGo4MbsSourceParameter(const char *name, Int_t id);

    virtual ~TGo4MbsSourceParameter();

    /** Define the Start index for the NextEvent */
    void SetStartEvent(UInt_t firstindex) { fuStartEvent = firstindex; }

    /** Returns the Start index for the NextEvent */
    UInt_t GetStartEvent() const { return fuStartEvent; }

    /** Define the index of Stop event */
    void SetStopEvent(UInt_t lastindex) { fuStopEvent = lastindex; }

    /** Returns the index of Stop event */
    UInt_t GetStopEvent() const { return fuStopEvent; }

    /** Defines event interval */
    void SetEventInterval(UInt_t skipinterval) { fuEventInterval = skipinterval; }

    /** Returns event interval */
    UInt_t GetEventInterval() const { return fuEventInterval; }

    /** Set optional port number, 0 or negative value means that default port number will be used */
    void SetPort(Int_t port) { fiPort = port; }

    /** Return optional port number, 0 or negative value means that default port number will be used */
    Int_t GetPort() const { return fiPort; }

    /** Set number of allowed reconnection retries */
    void SetRetryCnt(Int_t cnt) { fiRetryCnt = cnt; }

    /** Return allowed number of reconnection retries */
    Int_t GetRetryCnt() const { return fiRetryCnt; }

    /** basic method to printout object */
    void Print(Option_t *opt = "") const override;

    /** update contents of parameter class with external object. */
    Bool_t UpdateFrom(TGo4Parameter *rhs) override;

  private:

    /** Index of first event to process. */
    UInt_t fuStartEvent{0};

    /** Index of last event to process. */
    UInt_t fuStopEvent{0};

    /** Number of events to skip from file in between two read events  */
    UInt_t fuEventInterval{0};

    /** Port number .  */
    Int_t fiPort{0};

    /** Allowed number of retries when connection lost */
    Int_t fiRetryCnt{0};

  ClassDefOverride(TGo4MbsSourceParameter,3)

};

#endif //TGO4MBSFILEPARAMETER_H
