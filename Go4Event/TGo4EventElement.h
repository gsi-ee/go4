// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4EVENTELEMENT_H
#define TGO4EVENTELEMENT_H

#include "TNamed.h"

class TGo4EventSource;
class TBranch;
class TTree;

/** The abstract base class for the data elements of which the
 * unpacked events (or detector structure data, resp) are composed.
 * Any event structure consists of eventelements that are linked back to
 * their parent element.
 * The user event class is the top parent level event element;
 * unlimited levels of event elements as subevents might be
 * managed by this user event.
 * The way of subelement organization (e.g.
 * user event class members, TObjArray,...) is not restricted here.
 * @interface
 * @author J. Adamczewski
 * @since 12/2000*/

class TGo4EventElement : public TNamed {
   public:

      TGo4EventElement();

      TGo4EventElement(const char* name);
      TGo4EventElement(const char* aName, const char* aTitle, Short_t aBaseCat=0);

      virtual ~TGo4EventElement(void);

      /** Switches the valid state of this event. */
      void SetValid(Bool_t on) { fbIsValid = on; }

      /** Returns the valid state of this event. */
      Bool_t IsValid() const { return fbIsValid; }

      /** Setter for the event source that is currently used by the Fill method. */
      void SetEventSource(TGo4EventSource * src) { fxEventSource = src; }

      /** Setter for the parent event structure reference. */
      void SetParent(TGo4EventElement * par) { fxParent = par; }

      void SetKeepContents(Bool_t on=kTRUE) { fbKeepContents = on; }

      inline TGo4EventElement* GetParent() const { return fxParent; }

      virtual TGo4EventElement* GetChild(const char* name);

      inline TGo4EventSource * GetEventSource() const { return fxEventSource; }

      /** Check if event source is valid. Tests the pointer for
       * zero, and if not zero, compares classname of the
       * event source with given string. To be used from the
       * Fill method and from the analysis method  init event classes. */
      Bool_t CheckEventSource(const char* classname);

      /** Method called by the event owner (analysis step) to fill the
       * event element from the set event source. Event source can
       * be the source of the analysis step (if this is a raw event) or
       * the event processor (if this is a reduced event).
       * Fill method has to specify how the event source writes to the
       * members of this event structure. Either by calling methods of
       * the source (like myevent.a=fxEventSource->GetA(); etc ), or by
       * direct copy to the member (like fxEventSource->FillMemoryAt(&myevent.a);) */
      virtual Int_t Fill();

      /** Method called on initialization of event classes.
       * User might check the correct type of the eventsource here and assign this
       * to a subclass pointer. */
      virtual Int_t Init();

      /** Method prints content of the event */
      virtual void PrintEvent();

      virtual void makeBranch(TBranch *parent);
      virtual void deactivate();
      virtual void activate();

      virtual void Clear(Option_t *t="");
      virtual void clearAll(Int_t) {}
      virtual Bool_t isComposed() { return kFALSE; }
      virtual Short_t getId() { return fIdentifier; }
      virtual void setDebug (Bool_t debug) { fDebug=debug; }
      virtual TGo4EventElement& operator[](Int_t) { return *this; }

      virtual void Print(Option_t* option = "") const;

      /** Use this method to map event structure with the Tree branch(es) */
      virtual void synchronizeWithTree(TTree *tree, TGo4EventElement** var_ptr = 0);

      virtual Int_t activateBranch(TBranch *branch, Int_t index=0, TGo4EventElement** var_ptr = 0);

   private:

      /** Indicates if this event is valid or not. If false, the event store
       * will not write the event.
       * We still stream this, since it may be used to flag validity of composite sub events*/
      Bool_t fbIsValid;

      /** The higher level event element that owns this instance
       * @supplierCardinality 1 */
      TGo4EventElement * fxParent; //!

      /** The external event source  instance which is used to fill this event structure
       * from the raw event. Lazy initialization as zero; the pointer is set by
       * analysis only for top level Event Elements with subevents. Might be exchanged
       * on the fly to switch the unpack of different subevents into the same
       * event structure.
       * @supplierCardinality 0..1*/
      TGo4EventSource * fxEventSource; //!

   protected:
      Short_t fIdentifier; // Identifier
      Bool_t fDebug; //! Debug level

      /** helper flag to supress Clear if event shall be kept*/
      Bool_t fbKeepContents; //!

   ClassDef(TGo4EventElement,3)
};

#endif //TGO4EVENTELEMENT_H
