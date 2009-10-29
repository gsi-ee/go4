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

#ifndef TGO4COMGETCURRENTEVENT_H
#define TGO4COMGETCURRENTEVENT_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Requests the current event as a sample from the
 * client. Object name is name of the analysis step that
 * owns the event. By default, the output event of
 * that step is taken.
 * If no such analysis step, the event is searched by name in the eventstructure folder.
 * Event will be filled in a temporary tree that is send
 * to the gui. Any user event display may apply this
 * command.
 * @author J. Adamczewski
 * @since 02-Jul-2002
 */
class TGo4ComGetCurrentEvent : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComGetCurrentEvent();

    TGo4ComGetCurrentEvent(const char* obname);

    virtual ~TGo4ComGetCurrentEvent();

    Int_t ExeCom();

     /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    virtual void Set(TGo4RemoteCommand* remcon);


    Bool_t IsOutputEvent() const { return fbOutputEvent; }

    void SetOutputEvent(Bool_t output=kTRUE) { fbOutputEvent=output; }

    Bool_t IsPrintoutOnly() const { return fbPrintoutOnly; }

    void SetPrintoutOnly(Bool_t on=kTRUE) { fbPrintoutOnly=on; }

    Bool_t IsTreeMode() const { return fbTreeMode; }

    void SetTreeMode(Bool_t on=kTRUE) { fbTreeMode=on; }

  private:
    /* we use integer for bool expressions due to root streamer problems */

    /** If true output event is searched, otherwise input event of step */
    Bool_t fbOutputEvent;

    /** If true, the event will not be sent but content is shown on analysis terminal*/
    Bool_t fbPrintoutOnly;

    /** If true, event will be put into tree before showing; otherwise use
        PrintEvent method or send event directly*/
    Bool_t fbTreeMode;

  ClassDef(TGo4ComGetCurrentEvent,1)
};


#endif //TGO4COMGETCURRENTEVENT_H



