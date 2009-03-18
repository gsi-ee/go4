#ifndef TGO4EVENTELEMENT_H
#define TGO4EVENTELEMENT_H

#include "TNamed.h"

// dummy include, to be removed later
#include "TGo4Log.h"

class TGo4EventSource;
class TGo4CompositeEvent;
class TBranch;

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

   TGo4EventElement(void);

   TGo4EventElement(const char* name);
   TGo4EventElement(const char* aName, const char* aTitle, Short_t aBaseCat=0);

   virtual ~TGo4EventElement(void);

   /** Switches the valid state of this event. */
   void SetValid(Bool_t on) { fbIsValid=on; }

   /** Returns the valid state of this event. */
   Bool_t IsValid() const { return fbIsValid; }

   /** Setter for the event source that is currently used by the Fill method. */
   void SetEventSource(TGo4EventSource * src) { fxEventSource=src; }

   /** Setter for the parent event structure reference. */
   void SetParent(TGo4EventElement * par) { fxParent=par; }

   TGo4EventElement* GetParent() const { return fxParent; }

   TGo4EventSource * GetEventSource() const { return fxEventSource; }

   /** Check if event source is valid. Tests the pointer for
     * zero, and if not zero, compares classname of the
     * event source with given string. To be used from the
     * Fill method and from the analysis method  init event classes. */
   Bool_t CheckEventSource(const char* classname);

   /** Method called by the event owner (analysis step) to fill the
     * event element from the set event source. Event source can
     * be the source of the analysis step (if this is a raw event) or the event processor (if this is a reduced event).
     * Fill method has to specify how the event source writes to the
     * members of this event structure. Either by calling methods of
     * the source (like myevent.a=fxEventSource->GetA(); etc ), or by
     * direct copy to the member (like fxEventSource->FillMemoryAt(&myevent.a);) */
   virtual Int_t Fill();

   /** Method called on initialization of event classes.
     * User might check the correct type of the eventsource here and assign this
     * to a subclass pointer. */
   virtual Int_t Init();

   /** Method called by the event owner (analysis step) to clear the
    * event element. */
   virtual void PrintEvent();

   virtual TGo4CompositeEvent* getSubEventElement() { return 0; }
   virtual void makeBranch(TBranch *parent);
   virtual Int_t activateBranch(TBranch *branch,Int_t splitLevel, Int_t init=0);
   virtual void deactivate();
   virtual void activate();

   virtual void Clear(Option_t *t="");
   virtual void clearAll(Int_t ){;}
   virtual Bool_t isComposed() { return kFALSE; }
   virtual Short_t getId() { return fIdentifier;}
   virtual void setDebug (Bool_t debug) { fDebug=debug;}
   virtual TGo4EventElement& operator[](Int_t){ return *this; }

   virtual void Print(Option_t* option = "") const;

  private:

   /** Indicates if this event is valid or not. If false, the event store
     * will not write the event. */
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
    Bool_t isActivated; //! IO synchrone flag
    Bool_t fDebug; //! Debug level

  ClassDef(TGo4EventElement,2)
};

#endif //TGO4EVENTELEMENT_H
